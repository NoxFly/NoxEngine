// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/effects/TAAEffect.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#include <cmath>

namespace Nox {

    // Halton sequence for jitter
    static float halton(int index, int base) {
        float result = 0.0f;
        float f = 1.0f / static_cast<float>(base);
        int i = index;
        while (i > 0) {
            result += f * static_cast<float>(i % base);
            i /= base;
            f /= static_cast<float>(base);
        }
        return result;
    }

    static constexpr std::string_view TAAFragmentSource = R"glsl(
    #version 460 core

    in vec2 vUV;
    out vec4 FragColor;

    uniform sampler2D uCurrentFrame;
    uniform sampler2D uHistoryFrame;
    uniform sampler2D uDepthTexture;
    uniform sampler2D uVelocityTexture;
    uniform float uBlendFactor;
    uniform vec2 uTexelSize;

    // Neighbourhood clamping for ghosting reduction
    vec3 clipToAABB(vec3 color, vec3 minBox, vec3 maxBox) {
        vec3 center = 0.5 * (minBox + maxBox);
        vec3 halfSize = 0.5 * (maxBox - minBox) + 0.001;
        vec3 offset = color - center;
        vec3 ts = abs(halfSize / max(abs(offset), vec3(0.0001)));
        float t = min(min(ts.x, ts.y), ts.z);
        return center + offset * clamp(t, 0.0, 1.0);
    }

    void main() {
        vec3 currentColor = texture(uCurrentFrame, vUV).rgb;

        // Sample velocity for motion rejection
        vec2 velocity = texture(uVelocityTexture, vUV).rg;
        vec2 historyUV = vUV - velocity;

        // Check if history UV is valid
        if (historyUV.x < 0.0 || historyUV.x > 1.0 ||
            historyUV.y < 0.0 || historyUV.y > 1.0) {
            FragColor = vec4(currentColor, 1.0);
            return;
        }

        vec3 historyColor = texture(uHistoryFrame, historyUV).rgb;

        // Neighbourhood clamping (3x3)
        vec3 minNeighbour = currentColor;
        vec3 maxNeighbour = currentColor;
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                vec3 s = texture(uCurrentFrame, vUV + vec2(x, y) * uTexelSize).rgb;
                minNeighbour = min(minNeighbour, s);
                maxNeighbour = max(maxNeighbour, s);
            }
        }

        historyColor = clipToAABB(historyColor, minNeighbour, maxNeighbour);

        // Blend
        vec3 result = mix(currentColor, historyColor, uBlendFactor);
        FragColor = vec4(result, 1.0);
    }
    )glsl";

    TAAEffect::~TAAEffect() {
        if (taaProgram_ != 0)     { glDeleteProgram(taaProgram_); }
        if (historyTex_ != 0)     { glDeleteTextures(1, &historyTex_); }
        if (prevHistoryTex_ != 0) { glDeleteTextures(1, &prevHistoryTex_); }
        if (historyFBO_ != 0)     { glDeleteFramebuffers(1, &historyFBO_); }
    }

    void TAAEffect::init(int width, int height) {
        taaProgram_ = compilePostProcessShader(TAAFragmentSource);
        if (taaProgram_ == 0) {
            NOX_LOG_ERROR("Failed to compile TAA shader");
            return;
        }

        // Create history textures (double-buffered)
        glCreateTextures(GL_TEXTURE_2D, 1, &historyTex_);
        glTextureStorage2D(historyTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(historyTex_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(historyTex_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(historyTex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(historyTex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glCreateTextures(GL_TEXTURE_2D, 1, &prevHistoryTex_);
        glTextureStorage2D(prevHistoryTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(prevHistoryTex_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(prevHistoryTex_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(prevHistoryTex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(prevHistoryTex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // FBO for writing history
        glCreateFramebuffers(1, &historyFBO_);
        glNamedFramebufferTexture(historyFBO_, GL_COLOR_ATTACHMENT0, historyTex_, 0);

        sampleIndex_ = 0;
        nextFrame();
    }

    void TAAEffect::resize(int width, int height) {
        if (historyTex_ != 0)     { glDeleteTextures(1, &historyTex_); }
        if (prevHistoryTex_ != 0) { glDeleteTextures(1, &prevHistoryTex_); }
        if (historyFBO_ != 0)     { glDeleteFramebuffers(1, &historyFBO_); }

        glCreateTextures(GL_TEXTURE_2D, 1, &historyTex_);
        glTextureStorage2D(historyTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(historyTex_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(historyTex_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(historyTex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(historyTex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glCreateTextures(GL_TEXTURE_2D, 1, &prevHistoryTex_);
        glTextureStorage2D(prevHistoryTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(prevHistoryTex_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(prevHistoryTex_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(prevHistoryTex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(prevHistoryTex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glCreateFramebuffers(1, &historyFBO_);
        glNamedFramebufferTexture(historyFBO_, GL_COLOR_ATTACHMENT0, historyTex_, 0);

        sampleIndex_ = 0;
    }

    void TAAEffect::apply(uint32_t inputTexture, uint32_t outputFBO,
                           int width, int height) {
        if (taaProgram_ == 0) { return; }

        // Write TAA result to output FBO
        glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
        glViewport(0, 0, width, height);

        glUseProgram(taaProgram_);

        glBindTextureUnit(0, inputTexture);
        glUniform1i(glGetUniformLocation(taaProgram_, "uCurrentFrame"), 0);

        glBindTextureUnit(1, prevHistoryTex_);
        glUniform1i(glGetUniformLocation(taaProgram_, "uHistoryFrame"), 1);

        if (depthTex_ != 0) {
            glBindTextureUnit(2, depthTex_);
            glUniform1i(glGetUniformLocation(taaProgram_, "uDepthTexture"), 2);
        }

        if (velocityTex_ != 0) {
            glBindTextureUnit(3, velocityTex_);
            glUniform1i(glGetUniformLocation(taaProgram_, "uVelocityTexture"), 3);
        }

        glUniform1f(glGetUniformLocation(taaProgram_, "uBlendFactor"), blendFactor_);
        glUniform2f(glGetUniformLocation(taaProgram_, "uTexelSize"),
                    1.0f / static_cast<float>(width),
                    1.0f / static_cast<float>(height));

        drawFullscreenQuad();

        // Copy current output to history for next frame
        glCopyImageSubData(inputTexture, GL_TEXTURE_2D, 0, 0, 0, 0,
                           prevHistoryTex_, GL_TEXTURE_2D, 0, 0, 0, 0,
                           width, height, 1);
    }

    void TAAEffect::setDepthTexture(uint32_t depthTex) {
        depthTex_ = depthTex;
    }

    void TAAEffect::setVelocityTexture(uint32_t velocityTex) {
        velocityTex_ = velocityTex;
    }

    void TAAEffect::nextFrame() {
        constexpr int NumSamples = 16;
        sampleIndex_ = (sampleIndex_ + 1) % NumSamples;

        // Halton(2,3) sequence for jitter
        jitter_[0] = halton(sampleIndex_ + 1, 2) - 0.5f;
        jitter_[1] = halton(sampleIndex_ + 1, 3) - 0.5f;
    }

} // namespace Nox
