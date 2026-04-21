// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/ClusteredLighting.hpp>
#include <NoxEngine/core/Logger.hpp>
#include <NoxEngine/scene/Transform.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cmath>

namespace Nox {

    // GPU-side light struct layout (std430)
    struct alignas(16) GpuPointLight {
        glm::vec4 positionAndRange;   // xyz = position, w = range
        glm::vec4 colorAndIntensity;  // xyz = color, w = intensity
    };

    // GPU-side cluster struct layout
    struct alignas(16) GpuCluster {
        glm::vec4 minPoint;
        glm::vec4 maxPoint;
    };

    // Cluster light assignment data
    struct ClusterLightData {
        uint32_t offset;
        uint32_t count;
    };

    static constexpr std::string_view ClusterAssignComputeSource = R"glsl(
    #version 460 core

    layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

    struct PointLight {
        vec4 positionAndRange;
        vec4 colorAndIntensity;
    };

    struct Cluster {
        vec4 minPoint;
        vec4 maxPoint;
    };

    layout(std430, binding = 0) readonly buffer LightBuffer {
        PointLight lights[];
    };

    layout(std430, binding = 1) readonly buffer ClusterAABBBuffer {
        Cluster clusters[];
    };

    layout(std430, binding = 2) writeonly buffer ClusterLightBuffer {
        uvec2 clusterLightData[];  // x = offset, y = count
    };

    layout(std430, binding = 3) writeonly buffer LightIndexBuffer {
        uint lightIndices[];
    };

    uniform int uNumLights;
    uniform mat4 uViewMatrix;

    bool sphereIntersectsAABB(vec3 center, float radius, vec3 aabbMin, vec3 aabbMax) {
        vec3 closest = clamp(center, aabbMin, aabbMax);
        float dist = distance(center, closest);
        return dist <= radius;
    }

    void main() {
        uint clusterIdx = gl_GlobalInvocationID.x;
        if (clusterIdx >= clusters.length()) return;

        Cluster c = clusters[clusterIdx];
        uint count = 0;
        uint offset = clusterIdx * 128; // max lights per cluster

        for (int i = 0; i < uNumLights && count < 128; ++i) {
            vec3 lightPos = (uViewMatrix * vec4(lights[i].positionAndRange.xyz, 1.0)).xyz;
            float range = lights[i].positionAndRange.w;

            if (sphereIntersectsAABB(lightPos, range, c.minPoint.xyz, c.maxPoint.xyz)) {
                lightIndices[offset + count] = uint(i);
                count++;
            }
        }

        clusterLightData[clusterIdx] = uvec2(offset, count);
    }
    )glsl";

    ClusteredLighting::~ClusteredLighting() {
        if (lightSSBO_ != 0)       { glDeleteBuffers(1, &lightSSBO_); }
        if (clusterSSBO_ != 0)     { glDeleteBuffers(1, &clusterSSBO_); }
        if (clusterAABBSSBO_ != 0) { glDeleteBuffers(1, &clusterAABBSSBO_); }
        if (lightIndexSSBO_ != 0)  { glDeleteBuffers(1, &lightIndexSSBO_); }
        if (assignProgram_ != 0)   { glDeleteProgram(assignProgram_); }
    }

    void ClusteredLighting::init(const Config& config) {
        config_ = config;

        int totalClusters = config_.gridSizeX * config_.gridSizeY * config_.gridSizeZ;

        // Light SSBO — up to 1024 lights
        glCreateBuffers(1, &lightSSBO_);
        glNamedBufferStorage(lightSSBO_, sizeof(GpuPointLight) * 1024,
                             nullptr, GL_DYNAMIC_STORAGE_BIT);

        // Cluster AABB SSBO
        glCreateBuffers(1, &clusterAABBSSBO_);
        glNamedBufferStorage(clusterAABBSSBO_, sizeof(GpuCluster) * totalClusters,
                             nullptr, GL_DYNAMIC_STORAGE_BIT);

        // Cluster light assignment SSBO
        glCreateBuffers(1, &clusterSSBO_);
        glNamedBufferStorage(clusterSSBO_, sizeof(ClusterLightData) * totalClusters,
                             nullptr, GL_DYNAMIC_STORAGE_BIT);

        // Light index SSBO
        glCreateBuffers(1, &lightIndexSSBO_);
        glNamedBufferStorage(lightIndexSSBO_,
                             sizeof(uint32_t) * totalClusters * config_.maxLightsPerCluster,
                             nullptr, GL_DYNAMIC_STORAGE_BIT);

        // Compile compute shader
        GLuint cs = glCreateShader(GL_COMPUTE_SHADER);
        const char* src = ClusterAssignComputeSource.data();
        auto len = static_cast<GLint>(ClusterAssignComputeSource.size());
        glShaderSource(cs, 1, &src, &len);
        glCompileShader(cs);

        GLint success = 0;
        glGetShaderiv(cs, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512]{};
            glGetShaderInfoLog(cs, sizeof(log), nullptr, log);
            NOX_LOG_ERROR("Clustered lighting compute shader error: {}", log);
            glDeleteShader(cs);
            return;
        }

        assignProgram_ = glCreateProgram();
        glAttachShader(assignProgram_, cs);
        glLinkProgram(assignProgram_);
        glDeleteShader(cs);

        initialized_ = true;
        NOX_LOG_INFO("ClusteredLighting initialized ({}x{}x{} = {} clusters)",
                     config_.gridSizeX, config_.gridSizeY, config_.gridSizeZ,
                     totalClusters);
    }

    void ClusteredLighting::update(const Math::Mat4& viewMatrix,
                                    [[maybe_unused]] const Math::Mat4& projMatrix,
                                    float near, float far,
                                    const std::vector<std::shared_ptr<PointLight>>& pointLights,
                                    int viewportWidth, int viewportHeight) {
        if (!initialized_) { return; }

        int totalClusters = config_.gridSizeX * config_.gridSizeY * config_.gridSizeZ;

        // Build cluster AABBs in view space
        std::vector<GpuCluster> clusterAABBs(totalClusters);

        float clusterW = static_cast<float>(viewportWidth) / static_cast<float>(config_.gridSizeX);
        float clusterH = static_cast<float>(viewportHeight) / static_cast<float>(config_.gridSizeY);

        // Logarithmic depth slicing
        [[maybe_unused]] float logRatio = std::log(far / near);

        for (int z = 0; z < config_.gridSizeZ; ++z) {
            float zNear = near * std::pow(far / near, static_cast<float>(z) / static_cast<float>(config_.gridSizeZ));
            float zFar  = near * std::pow(far / near, static_cast<float>(z + 1) / static_cast<float>(config_.gridSizeZ));

            for (int y = 0; y < config_.gridSizeY; ++y) {
                for (int x = 0; x < config_.gridSizeX; ++x) {
                    int idx = x + y * config_.gridSizeX + z * config_.gridSizeX * config_.gridSizeY;

                    // Screen-space bounds to view-space (simplified)
                    float screenLeft = static_cast<float>(x) * clusterW;
                    float screenRight = static_cast<float>(x + 1) * clusterW;
                    float screenBottom = static_cast<float>(y) * clusterH;
                    float screenTop = static_cast<float>(y + 1) * clusterH;

                    // Convert to NDC
                    float ndcLeft   = screenLeft / static_cast<float>(viewportWidth) * 2.0f - 1.0f;
                    float ndcRight  = screenRight / static_cast<float>(viewportWidth) * 2.0f - 1.0f;
                    float ndcBottom = screenBottom / static_cast<float>(viewportHeight) * 2.0f - 1.0f;
                    float ndcTop    = screenTop / static_cast<float>(viewportHeight) * 2.0f - 1.0f;

                    clusterAABBs[idx].minPoint = glm::vec4(ndcLeft, ndcBottom, -zFar, 1.0f);
                    clusterAABBs[idx].maxPoint = glm::vec4(ndcRight, ndcTop, -zNear, 1.0f);
                }
            }
        }

        glNamedBufferSubData(clusterAABBSSBO_, 0,
                             sizeof(GpuCluster) * totalClusters,
                             clusterAABBs.data());

        // Upload point lights
        auto numLights = static_cast<int>(std::min(pointLights.size(), static_cast<size_t>(1024)));
        std::vector<GpuPointLight> gpuLights(numLights);

        for (int i = 0; i < numLights; ++i) {
            const auto& light = pointLights[i];
            gpuLights[i].positionAndRange = glm::vec4(light->transform().position(), light->range());
            gpuLights[i].colorAndIntensity = glm::vec4(
                light->color().r, light->color().g, light->color().b,
                light->intensity()
            );
        }

        glNamedBufferSubData(lightSSBO_, 0,
                             sizeof(GpuPointLight) * numLights,
                             gpuLights.data());

        // Dispatch compute shader for light assignment
        glUseProgram(assignProgram_);
        glUniform1i(glGetUniformLocation(assignProgram_, "uNumLights"), numLights);
        glUniformMatrix4fv(glGetUniformLocation(assignProgram_, "uViewMatrix"),
                           1, GL_FALSE, glm::value_ptr(viewMatrix));

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightSSBO_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, clusterAABBSSBO_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, clusterSSBO_);

        glDispatchCompute(totalClusters, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void ClusteredLighting::bind() const {
        if (!initialized_) { return; }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightSSBO_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, clusterAABBSSBO_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, clusterSSBO_);
    }

} // namespace Nox
