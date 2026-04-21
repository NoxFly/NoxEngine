// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/OcclusionCulling.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

namespace Nox {

    // ── HZB downsample compute shader ──────────────────────────────

    static constexpr std::string_view HZBDownsampleSource = R"glsl(
    #version 460 core
    layout(local_size_x = 8, local_size_y = 8) in;

    layout(binding = 0, r32f) readonly uniform image2D uSrcLevel;
    layout(binding = 1, r32f) writeonly uniform image2D uDstLevel;

    void main() {
        ivec2 dstCoord = ivec2(gl_GlobalInvocationID.xy);
        ivec2 srcCoord = dstCoord * 2;

        float d0 = imageLoad(uSrcLevel, srcCoord).r;
        float d1 = imageLoad(uSrcLevel, srcCoord + ivec2(1, 0)).r;
        float d2 = imageLoad(uSrcLevel, srcCoord + ivec2(0, 1)).r;
        float d3 = imageLoad(uSrcLevel, srcCoord + ivec2(1, 1)).r;

        // Use max depth for conservative occlusion (reverse-Z: use min for standard)
        float maxDepth = max(max(d0, d1), max(d2, d3));
        imageStore(uDstLevel, dstCoord, vec4(maxDepth));
    }
    )glsl";

    // ── AABB test compute shader ───────────────────────────────────

    static constexpr std::string_view AABBTestSource = R"glsl(
    #version 460 core
    layout(local_size_x = 64) in;

    struct AABB {
        vec4 minPoint; // .w unused
        vec4 maxPoint; // .w unused
    };

    layout(std430, binding = 0) readonly buffer AABBBuffer {
        AABB aabbs[];
    };

    layout(std430, binding = 1) writeonly buffer ResultBuffer {
        uint visibility[];
    };

    uniform mat4 uViewProj;
    uniform sampler2D uHZB;
    uniform vec2 uScreenSize;

    void main() {
        uint idx = gl_GlobalInvocationID.x;
        if (idx >= aabbs.length()) return;

        AABB box = aabbs[idx];

        // Project 8 AABB corners to screen space
        vec2 minScreen = vec2(1e10);
        vec2 maxScreen = vec2(-1e10);
        float nearZ = 1.0;

        for (int i = 0; i < 8; i++) {
            vec3 corner = vec3(
                (i & 1) != 0 ? box.maxPoint.x : box.minPoint.x,
                (i & 2) != 0 ? box.maxPoint.y : box.minPoint.y,
                (i & 4) != 0 ? box.maxPoint.z : box.minPoint.z
            );

            vec4 clip = uViewProj * vec4(corner, 1.0);
            if (clip.w <= 0.0) {
                visibility[idx] = 1u; // Behind camera, assume visible
                return;
            }

            vec3 ndc = clip.xyz / clip.w;
            vec2 screen = ndc.xy * 0.5 + 0.5;

            minScreen = min(minScreen, screen);
            maxScreen = max(maxScreen, screen);
            nearZ = min(nearZ, ndc.z * 0.5 + 0.5);
        }

        // Clamp to screen
        minScreen = clamp(minScreen, vec2(0.0), vec2(1.0));
        maxScreen = clamp(maxScreen, vec2(0.0), vec2(1.0));

        // Determine HZB mip level based on projected size
        vec2 sizePixels = (maxScreen - minScreen) * uScreenSize;
        float maxDim = max(sizePixels.x, sizePixels.y);
        float mipLevel = ceil(log2(maxDim));
        mipLevel = max(mipLevel, 0.0);

        // Sample HZB at the computed mip level
        vec2 center = (minScreen + maxScreen) * 0.5;
        float hzbDepth = textureLod(uHZB, center, mipLevel).r;

        visibility[idx] = (nearZ <= hzbDepth) ? 1u : 0u;
    }
    )glsl";

    OcclusionCulling::~OcclusionCulling() {
        destroy();
    }

    void OcclusionCulling::init(int depthWidth, int depthHeight) {
        width_ = depthWidth;
        height_ = depthHeight;
        numLevels_ = static_cast<int>(std::floor(std::log2(std::max(depthWidth, depthHeight)))) + 1;

        // Create HZB texture with mip chain
        glCreateTextures(GL_TEXTURE_2D, 1, &hzbTexture_);
        glTextureStorage2D(hzbTexture_, numLevels_, GL_R32F, width_, height_);
        glTextureParameteri(hzbTexture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTextureParameteri(hzbTexture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(hzbTexture_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(hzbTexture_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glCreateFramebuffers(1, &hzbFBO_);

        // Compile compute shaders
        auto compileCompute = [](std::string_view src) -> GLuint {
            GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
            const char* srcPtr = src.data();
            auto len = static_cast<GLint>(src.size());
            glShaderSource(shader, 1, &srcPtr, &len);
            glCompileShader(shader);
            GLint success = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char log[512]{};
                glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
                NOX_LOG_ERROR("OcclusionCulling shader error: {}", log);
                glDeleteShader(shader);
                return 0;
            }
            GLuint prog = glCreateProgram();
            glAttachShader(prog, shader);
            glLinkProgram(prog);
            glDeleteShader(shader);
            return prog;
        };

        downsampleProgram_ = compileCompute(HZBDownsampleSource);
        testProgram_ = compileCompute(AABBTestSource);

        // Create result SSBO
        glCreateBuffers(1, &resultSSBO_);

        initialized_ = true;
        NOX_LOG_INFO("OcclusionCulling initialized ({}x{}, {} mip levels)",
                     width_, height_, numLevels_);
    }

    void OcclusionCulling::resize(int depthWidth, int depthHeight) {
        destroy();
        init(depthWidth, depthHeight);
    }

    void OcclusionCulling::buildHZB(uint32_t depthTexture) {
        if (!initialized_ || !enabled_ || downsampleProgram_ == 0) { return; }

        // Copy depth buffer to HZB level 0
        glCopyImageSubData(depthTexture, GL_TEXTURE_2D, 0, 0, 0, 0,
                           hzbTexture_, GL_TEXTURE_2D, 0, 0, 0, 0,
                           width_, height_, 1);

        // Generate mip chain
        glUseProgram(downsampleProgram_);

        int w = width_;
        int h = height_;

        for (int level = 1; level < numLevels_; ++level) {
            int srcW = w;
            int srcH = h;
            w = std::max(1, w / 2);
            h = std::max(1, h / 2);

            glBindImageTexture(0, hzbTexture_, level - 1, GL_FALSE, 0,
                               GL_READ_ONLY, GL_R32F);
            glBindImageTexture(1, hzbTexture_, level, GL_FALSE, 0,
                               GL_WRITE_ONLY, GL_R32F);

            glDispatchCompute(
                static_cast<GLuint>((w + 7) / 8),
                static_cast<GLuint>((h + 7) / 8),
                1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            (void)srcW;
            (void)srcH;
        }
    }

    bool OcclusionCulling::testAABB(const Math::AABB& worldAABB,
                                     const Math::Mat4& viewProjMatrix) const {
        if (!initialized_ || !enabled_) { return true; }

        // Project 8 corners of AABB to screen space
        Math::Vec2 minScreen(std::numeric_limits<float>::max());
        Math::Vec2 maxScreen(std::numeric_limits<float>::lowest());
        float nearZ = 1.0f;

        for (int i = 0; i < 8; ++i) {
            Math::Vec3 corner(
                (i & 1) ? worldAABB.max.x : worldAABB.min.x,
                (i & 2) ? worldAABB.max.y : worldAABB.min.y,
                (i & 4) ? worldAABB.max.z : worldAABB.min.z
            );

            Math::Vec4 clip = viewProjMatrix * Math::Vec4(corner, 1.0f);
            if (clip.w <= 0.0f) { return true; } // Behind camera

            Math::Vec3 ndc = Math::Vec3(clip) / clip.w;
            Math::Vec2 screen = Math::Vec2(ndc) * 0.5f + 0.5f;

            minScreen = glm::min(minScreen, screen);
            maxScreen = glm::max(maxScreen, screen);
            nearZ = std::min(nearZ, ndc.z * 0.5f + 0.5f);
        }

        // Determine mip level
        Math::Vec2 sizePixels = (maxScreen - minScreen) * Math::Vec2(
            static_cast<float>(width_), static_cast<float>(height_));
        float maxDim = std::max(sizePixels.x, sizePixels.y);
        int mipLevel = std::clamp(
            static_cast<int>(std::ceil(std::log2(maxDim))),
            0, numLevels_ - 1);

        // Read HZB depth at the center pixel
        Math::Vec2 center = (minScreen + maxScreen) * 0.5f;
        int mipW = std::max(1, width_ >> mipLevel);
        int mipH = std::max(1, height_ >> mipLevel);
        int px = std::clamp(static_cast<int>(center.x * static_cast<float>(mipW)), 0, mipW - 1);
        int py = std::clamp(static_cast<int>(center.y * static_cast<float>(mipH)), 0, mipH - 1);

        float hzbDepth = 0.0f;
        glGetTextureSubImage(hzbTexture_, mipLevel,
                             px, py, 0, 1, 1, 1,
                             GL_RED, GL_FLOAT, sizeof(float), &hzbDepth);

        return nearZ <= hzbDepth;
    }

    void OcclusionCulling::testAABBs(const std::vector<Math::AABB>& aabbs,
                                      const Math::Mat4& viewProjMatrix,
                                      std::vector<bool>& outVisibility) {
        if (!initialized_ || !enabled_ || testProgram_ == 0) {
            outVisibility.assign(aabbs.size(), true);
            return;
        }

        auto count = static_cast<int>(aabbs.size());
        outVisibility.resize(static_cast<size_t>(count));

        // Upload AABBs to SSBO
        struct GPUAabb {
            Math::Vec4 minPt;
            Math::Vec4 maxPt;
        };

        std::vector<GPUAabb> gpuAabbs(static_cast<size_t>(count));
        for (int i = 0; i < count; ++i) {
            gpuAabbs[i].minPt = Math::Vec4(aabbs[i].min, 0.0f);
            gpuAabbs[i].maxPt = Math::Vec4(aabbs[i].max, 0.0f);
        }

        GLuint aabbSSBO = 0;
        glCreateBuffers(1, &aabbSSBO);
        glNamedBufferStorage(aabbSSBO,
                             static_cast<GLsizeiptr>(gpuAabbs.size() * sizeof(GPUAabb)),
                             gpuAabbs.data(), 0);

        // Result buffer
        std::vector<uint32_t> results(static_cast<size_t>(count), 0);
        glDeleteBuffers(1, &resultSSBO_);
        glCreateBuffers(1, &resultSSBO_);
        glNamedBufferStorage(resultSSBO_,
                             static_cast<GLsizeiptr>(results.size() * sizeof(uint32_t)),
                             results.data(), GL_MAP_READ_BIT);

        glUseProgram(testProgram_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, aabbSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, resultSSBO_);

        glUniformMatrix4fv(glGetUniformLocation(testProgram_, "uViewProj"),
                           1, GL_FALSE, glm::value_ptr(viewProjMatrix));
        glUniform2f(glGetUniformLocation(testProgram_, "uScreenSize"),
                    static_cast<float>(width_), static_cast<float>(height_));

        glBindTextureUnit(0, hzbTexture_);
        glUniform1i(glGetUniformLocation(testProgram_, "uHZB"), 0);

        glDispatchCompute(static_cast<GLuint>((count + 63) / 64), 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // Read back results
        auto* mapped = static_cast<const uint32_t*>(
            glMapNamedBufferRange(resultSSBO_, 0,
                                  static_cast<GLsizeiptr>(count * sizeof(uint32_t)),
                                  GL_MAP_READ_BIT));
        if (mapped) {
            for (int i = 0; i < count; ++i) {
                outVisibility[i] = (mapped[i] != 0);
            }
            glUnmapNamedBuffer(resultSSBO_);
        }
        else {
            std::fill(outVisibility.begin(), outVisibility.end(), true);
        }

        glDeleteBuffers(1, &aabbSSBO);
    }

    void OcclusionCulling::destroy() {
        if (hzbTexture_ != 0) { glDeleteTextures(1, &hzbTexture_); hzbTexture_ = 0; }
        if (hzbFBO_ != 0) { glDeleteFramebuffers(1, &hzbFBO_); hzbFBO_ = 0; }
        if (downsampleProgram_ != 0) { glDeleteProgram(downsampleProgram_); downsampleProgram_ = 0; }
        if (testProgram_ != 0) { glDeleteProgram(testProgram_); testProgram_ = 0; }
        if (resultSSBO_ != 0) { glDeleteBuffers(1, &resultSSBO_); resultSSBO_ = 0; }
        initialized_ = false;
    }

} // namespace Nox
