// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/effects/SSAOEffect.hpp>

#include <GL/glew.h>

#include <array>
#include <cmath>
#include <random>
#include <string>

namespace Nox {

    SSAOEffect::~SSAOEffect() {
        destroyResources();
    }

    void SSAOEffect::init(int width, int height) {
        ssaoProgram_    = compilePostProcessShader(SSAOFragSource);
        blurProgram_    = compilePostProcessShader(BlurFragSource);
        combineProgram_ = compilePostProcessShader(CombineFragSource);

        // SSAO pass uniforms
        ssaoLocDepth_      = glGetUniformLocation(ssaoProgram_, "uDepth");
        ssaoLocNoise_      = glGetUniformLocation(ssaoProgram_, "uNoise");
        ssaoLocProjection_ = glGetUniformLocation(ssaoProgram_, "uProjection");
        ssaoLocSamples_    = glGetUniformLocation(ssaoProgram_, "uSamples");
        ssaoLocRadius_     = glGetUniformLocation(ssaoProgram_, "uRadius");
        ssaoLocBias_       = glGetUniformLocation(ssaoProgram_, "uBias");
        ssaoLocScreenSize_ = glGetUniformLocation(ssaoProgram_, "uScreenSize");
        ssaoLocKernelSize_ = glGetUniformLocation(ssaoProgram_, "uKernelSize");
        ssaoLocPower_      = glGetUniformLocation(ssaoProgram_, "uPower");

        // Blur pass uniforms
        blurLocTexture_ = glGetUniformLocation(blurProgram_, "uTexture");

        // Combine pass uniforms
        combLocScene_ = glGetUniformLocation(combineProgram_, "uScene");
        combLocAO_    = glGetUniformLocation(combineProgram_, "uAO");

        generateKernel();
        generateNoiseTexture();
        createResources(width, height);
    }

    void SSAOEffect::resize(int width, int height) {
        if (width == width_ && height == height_) {
            return;
        }

        destroyResources();
        createResources(width, height);
    }

    void SSAOEffect::apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) {
        if (depthTexture_ == 0 || projMatrix_ == nullptr) {
            return;
        }

        // 1) SSAO pass — render occlusion to single-channel texture
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO_);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glUseProgram(ssaoProgram_);
        glBindTextureUnit(0, depthTexture_);
        glBindTextureUnit(1, noiseTexture_);
        glUniform1i(ssaoLocDepth_, 0);
        glUniform1i(ssaoLocNoise_, 1);
        glUniformMatrix4fv(ssaoLocProjection_, 1, GL_FALSE, projMatrix_);
        glUniform3fv(ssaoLocSamples_, kernelSize_, kernelData_);
        glUniform1f(ssaoLocRadius_, radius_);
        glUniform1f(ssaoLocBias_, bias_);
        glUniform2f(ssaoLocScreenSize_, static_cast<float>(width), static_cast<float>(height));
        glUniform1i(ssaoLocKernelSize_, kernelSize_);
        glUniform1f(ssaoLocPower_, occlusionPower_);
        drawFullscreenQuad();

        // 2) Blur pass — smooth the AO result
        glBindFramebuffer(GL_FRAMEBUFFER, blurFBO_);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(blurProgram_);
        glBindTextureUnit(0, ssaoTexture_);
        glUniform1i(blurLocTexture_, 0);
        drawFullscreenQuad();

        // 3) Combine — multiply AO into the scene
        glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(combineProgram_);
        glBindTextureUnit(0, inputTexture);
        glBindTextureUnit(1, blurTexture_);
        glUniform1i(combLocScene_, 0);
        glUniform1i(combLocAO_, 1);
        drawFullscreenQuad();
    }

    void SSAOEffect::createResources(int width, int height) {
        width_  = width;
        height_ = height;

        // SSAO FBO — single-channel (RED) float texture
        glCreateFramebuffers(1, &ssaoFBO_);
        glCreateTextures(GL_TEXTURE_2D, 1, &ssaoTexture_);
        glTextureStorage2D(ssaoTexture_, 1, GL_R16F, width, height);
        glTextureParameteri(ssaoTexture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(ssaoTexture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(ssaoTexture_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(ssaoTexture_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glNamedFramebufferTexture(ssaoFBO_, GL_COLOR_ATTACHMENT0, ssaoTexture_, 0);

        // Blur FBO — single-channel
        glCreateFramebuffers(1, &blurFBO_);
        glCreateTextures(GL_TEXTURE_2D, 1, &blurTexture_);
        glTextureStorage2D(blurTexture_, 1, GL_R16F, width, height);
        glTextureParameteri(blurTexture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(blurTexture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(blurTexture_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(blurTexture_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glNamedFramebufferTexture(blurFBO_, GL_COLOR_ATTACHMENT0, blurTexture_, 0);
    }

    void SSAOEffect::destroyResources() {
        if (ssaoFBO_ != 0) {
            glDeleteFramebuffers(1, &ssaoFBO_);
            glDeleteTextures(1, &ssaoTexture_);
            ssaoFBO_ = 0;
            ssaoTexture_ = 0;
        }

        if (blurFBO_ != 0) {
            glDeleteFramebuffers(1, &blurFBO_);
            glDeleteTextures(1, &blurTexture_);
            blurFBO_ = 0;
            blurTexture_ = 0;
        }

        if (noiseTexture_ != 0) {
            glDeleteTextures(1, &noiseTexture_);
            noiseTexture_ = 0;
        }

        if (ssaoProgram_ != 0) {
            glDeleteProgram(ssaoProgram_);
            ssaoProgram_ = 0;
        }
        if (blurProgram_ != 0) {
            glDeleteProgram(blurProgram_);
            blurProgram_ = 0;
        }
        if (combineProgram_ != 0) {
            glDeleteProgram(combineProgram_);
            combineProgram_ = 0;
        }
    }

    void SSAOEffect::generateKernel() {
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        for (int i = 0; i < MaxKernelSize; ++i) {
            // Random point in hemisphere (tangent space, z-up)
            float x = dist(rng) * 2.0f - 1.0f;
            float y = dist(rng) * 2.0f - 1.0f;
            float z = dist(rng); // hemisphere: z >= 0

            // Normalize
            float len = std::sqrt(x * x + y * y + z * z);
            if (len > 0.0001f) {
                x /= len;
                y /= len;
                z /= len;
            }

            // Scale: distribute more samples closer to the origin
            float scale = static_cast<float>(i) / static_cast<float>(MaxKernelSize);
            scale = 0.1f + scale * scale * 0.9f; // lerp(0.1, 1.0, scale^2)
            x *= scale;
            y *= scale;
            z *= scale;

            kernelData_[i * 3 + 0] = x;
            kernelData_[i * 3 + 1] = y;
            kernelData_[i * 3 + 2] = z;
        }
    }

    void SSAOEffect::generateNoiseTexture() {
        std::mt19937 rng(123);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        // 4x4 noise texture — random tangent-space rotation vectors
        constexpr int NoiseSize = 4;
        std::array<float, NoiseSize * NoiseSize * 3> noiseData{};

        for (int i = 0; i < NoiseSize * NoiseSize; ++i) {
            noiseData[i * 3 + 0] = dist(rng) * 2.0f - 1.0f;
            noiseData[i * 3 + 1] = dist(rng) * 2.0f - 1.0f;
            noiseData[i * 3 + 2] = 0.0f; // rotate around z-axis in tangent space
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &noiseTexture_);
        glTextureStorage2D(noiseTexture_, 1, GL_RGB16F, NoiseSize, NoiseSize);
        glTextureSubImage2D(noiseTexture_, 0, 0, 0, NoiseSize, NoiseSize,
                            GL_RGB, GL_FLOAT, noiseData.data());
        glTextureParameteri(noiseTexture_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(noiseTexture_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(noiseTexture_, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(noiseTexture_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

} // namespace Nox
