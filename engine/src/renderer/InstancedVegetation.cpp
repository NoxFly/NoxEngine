// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/InstancedVegetation.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <random>

namespace Nox {

    // ── Instanced vegetation shaders ───────────────────────────────

    static constexpr std::string_view VegVertexSource = R"glsl(
    #version 460 core

    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aUV;
    // Instance transform (mat4 = 4 vec4 columns)
    layout(location = 3) in vec4 aInstCol0;
    layout(location = 4) in vec4 aInstCol1;
    layout(location = 5) in vec4 aInstCol2;
    layout(location = 6) in vec4 aInstCol3;

    uniform mat4 uView;
    uniform mat4 uProj;

    out vec3 vWorldPos;
    out vec3 vNormal;
    out vec2 vUV;

    void main() {
        mat4 model = mat4(aInstCol0, aInstCol1, aInstCol2, aInstCol3);
        vec4 worldPos = model * vec4(aPosition, 1.0);
        vWorldPos = worldPos.xyz;
        vNormal = mat3(transpose(inverse(model))) * aNormal;
        vUV = aUV;
        gl_Position = uProj * uView * worldPos;
    }
    )glsl";

    static constexpr std::string_view VegFragmentSource = R"glsl(
    #version 460 core

    in vec3 vWorldPos;
    in vec3 vNormal;
    in vec2 vUV;

    out vec4 FragColor;

    void main() {
        vec3 N = normalize(vNormal);
        vec3 L = normalize(vec3(-0.5, -1.0, -0.3));
        float NdotL = max(dot(N, -L), 0.0);

        // Simple green with lighting
        vec3 color = vec3(0.2, 0.5, 0.15) * (NdotL * 0.8 + 0.2);
        FragColor = vec4(color, 1.0);
    }
    )glsl";

    InstancedVegetation::~InstancedVegetation() {
        if (instanceVBO_ != 0) { glDeleteBuffers(1, &instanceVBO_); }
        if (instanceVAO_ != 0) { glDeleteVertexArrays(1, &instanceVAO_); }
        if (instanceProgram_ != 0) { glDeleteProgram(instanceProgram_); }
    }

    void InstancedVegetation::init(const std::shared_ptr<Mesh>& prototype,
                                    const Config& config) {
        prototype_ = prototype;
        config_ = config;

        // Compile shaders
        auto compileShader = [](GLenum type, std::string_view src) -> GLuint {
            GLuint shader = glCreateShader(type);
            const char* srcPtr = src.data();
            auto len = static_cast<GLint>(src.size());
            glShaderSource(shader, 1, &srcPtr, &len);
            glCompileShader(shader);
            GLint success = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char log[512]{};
                glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
                NOX_LOG_ERROR("Vegetation shader error: {}", log);
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        };

        GLuint vs = compileShader(GL_VERTEX_SHADER, VegVertexSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, VegFragmentSource);

        if (vs != 0 && fs != 0) {
            instanceProgram_ = glCreateProgram();
            glAttachShader(instanceProgram_, vs);
            glAttachShader(instanceProgram_, fs);
            glLinkProgram(instanceProgram_);
        }
        if (vs) { glDeleteShader(vs); }
        if (fs) { glDeleteShader(fs); }

        initialized_ = true;
        NOX_LOG_INFO("InstancedVegetation initialized (density={}, maxInstances={})",
                     config_.density, config_.maxInstances);
    }

    void InstancedVegetation::generateFromTerrain(const std::vector<float>& heightmap,
                                                    int hmWidth, int hmHeight,
                                                    float terrainScale, float heightScale) {
        allTransforms_.clear();

        std::mt19937 rng(42);
        std::uniform_real_distribution<float> offsetDist(0.0f, 1.0f);
        std::uniform_real_distribution<float> scaleDist(config_.minScale, config_.maxScale);
        std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);

        float spacing = 1.0f / std::sqrt(config_.density);

        int placedCount = 0;
        float worldW = static_cast<float>(hmWidth) * terrainScale;
        float worldH = static_cast<float>(hmHeight) * terrainScale;

        for (float z = 0.0f; z < worldH && placedCount < config_.maxInstances; z += spacing) {
            for (float x = 0.0f; x < worldW && placedCount < config_.maxInstances; x += spacing) {
                // Jitter position
                float jx = x + (offsetDist(rng) - 0.5f) * spacing;
                float jz = z + (offsetDist(rng) - 0.5f) * spacing;

                // Sample heightmap
                int ix = std::clamp(static_cast<int>(jx / terrainScale), 0, hmWidth - 2);
                int iz = std::clamp(static_cast<int>(jz / terrainScale), 0, hmHeight - 2);
                float hy = heightmap[static_cast<size_t>(iz * hmWidth + ix)];

                // Check slope
                float hR = heightmap[static_cast<size_t>(iz * hmWidth + ix + 1)];
                float hU = heightmap[static_cast<size_t>((iz + 1) * hmWidth + ix)];
                Math::Vec3 normal = glm::normalize(Math::Vec3(hy - hR, terrainScale, hy - hU));
                float slopeAngle = glm::degrees(std::acos(std::clamp(normal.y, 0.0f, 1.0f)));

                if (slopeAngle > config_.maxSlopeAngle) {
                    continue;
                }

                // Build transform
                float scale = scaleDist(rng);
                float rotation = rotDist(rng);

                Math::Mat4 model(1.0f);
                model = glm::translate(model, Math::Vec3(jx, hy, jz));
                model = glm::rotate(model, glm::radians(rotation), Math::Vec3(0.0f, 1.0f, 0.0f));
                model = glm::scale(model, Math::Vec3(scale));

                allTransforms_.push_back(model);
                ++placedCount;
            }
        }

        totalInstances_ = placedCount;
        uploadInstanceData();

        (void)heightScale;
        NOX_LOG_INFO("Vegetation: placed {} instances", placedCount);
    }

    void InstancedVegetation::setInstanceTransforms(const std::vector<Math::Mat4>& transforms) {
        allTransforms_ = transforms;
        totalInstances_ = static_cast<int>(transforms.size());
        uploadInstanceData();
    }

    void InstancedVegetation::update(const Math::Vec3& cameraPosition,
                                      [[maybe_unused]] const Math::Mat4& viewProjMatrix) {
        if (!initialized_) { return; }

        visibleTransforms_.clear();
        float maxDist2 = config_.drawDistance * config_.drawDistance;

        for (const auto& t : allTransforms_) {
            Math::Vec3 pos(t[3]); // Extract position from transform
            float dist2 = glm::dot(pos - cameraPosition, pos - cameraPosition);
            if (dist2 <= maxDist2) {
                visibleTransforms_.push_back(t);
            }
        }

        visibleInstances_ = static_cast<int>(visibleTransforms_.size());

        // Update GPU buffer with visible instances
        if (instanceVBO_ != 0 && !visibleTransforms_.empty()) {
            auto bufSize = static_cast<GLsizeiptr>(visibleTransforms_.size() * sizeof(Math::Mat4));
            glNamedBufferSubData(instanceVBO_, 0,
                                 std::min(bufSize,
                                 static_cast<GLsizeiptr>(config_.maxInstances * sizeof(Math::Mat4))),
                                 visibleTransforms_.data());
        }
    }

    void InstancedVegetation::render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix) {
        if (!initialized_ || visibleInstances_ == 0 || instanceProgram_ == 0) { return; }

        glUseProgram(instanceProgram_);
        glUniformMatrix4fv(glGetUniformLocation(instanceProgram_, "uView"),
                           1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(instanceProgram_, "uProj"),
                           1, GL_FALSE, glm::value_ptr(projMatrix));

        glBindVertexArray(instanceVAO_);
        // We assume the prototype mesh has been bound into this VAO
        // In a full implementation, we'd bind the prototype mesh's VBO here
        // For now, draw as instanced points or quads
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, visibleInstances_);
    }

    void InstancedVegetation::uploadInstanceData() {
        if (allTransforms_.empty()) { return; }

        if (instanceVBO_ == 0) {
            glCreateBuffers(1, &instanceVBO_);
        }

        auto maxSize = static_cast<GLsizeiptr>(config_.maxInstances * sizeof(Math::Mat4));
        auto dataSize = static_cast<GLsizeiptr>(allTransforms_.size() * sizeof(Math::Mat4));

        glDeleteBuffers(1, &instanceVBO_);
        glCreateBuffers(1, &instanceVBO_);
        glNamedBufferStorage(instanceVBO_, maxSize, nullptr,
                             GL_DYNAMIC_STORAGE_BIT);
        glNamedBufferSubData(instanceVBO_, 0,
                             std::min(dataSize, maxSize),
                             allTransforms_.data());

        // Setup VAO with instanced attributes
        if (instanceVAO_ == 0) {
            glCreateVertexArrays(1, &instanceVAO_);
        }

        glVertexArrayVertexBuffer(instanceVAO_, 1, instanceVBO_, 0,
                                  static_cast<GLsizei>(sizeof(Math::Mat4)));

        // Mat4 = 4 vec4 columns at locations 3, 4, 5, 6
        for (int i = 0; i < 4; ++i) {
            GLuint loc = 3 + static_cast<GLuint>(i);
            glEnableVertexArrayAttrib(instanceVAO_, loc);
            glVertexArrayAttribFormat(instanceVAO_, loc, 4, GL_FLOAT, GL_FALSE,
                                      static_cast<GLuint>(i * sizeof(Math::Vec4)));
            glVertexArrayAttribBinding(instanceVAO_, loc, 1);
            glVertexArrayBindingDivisor(instanceVAO_, 1, 1);
        }
    }

} // namespace Nox
