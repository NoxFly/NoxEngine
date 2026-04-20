// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/effects/BloomEffect.hpp>

#include <GL/glew.h>

namespace Nox {

    BloomEffect::~BloomEffect() {
        destroyResources();
    }

    void BloomEffect::init(int width, int height) {
        brightProgram_  = compilePostProcessShader(BrightExtractSource);
        blurHProgram_   = compilePostProcessShader(BlurHorizontalSource);
        blurVProgram_   = compilePostProcessShader(BlurVerticalSource);
        combineProgram_ = compilePostProcessShader(CombineSource);

        brightLocTexture_   = glGetUniformLocation(brightProgram_, "uTexture");
        brightLocThreshold_ = glGetUniformLocation(brightProgram_, "uThreshold");
        blurHLocTexture_    = glGetUniformLocation(blurHProgram_, "uTexture");
        blurHLocTexelSize_  = glGetUniformLocation(blurHProgram_, "uTexelSize");
        blurVLocTexture_    = glGetUniformLocation(blurVProgram_, "uTexture");
        blurVLocTexelSize_  = glGetUniformLocation(blurVProgram_, "uTexelSize");
        combLocScene_       = glGetUniformLocation(combineProgram_, "uScene");
        combLocBloom_       = glGetUniformLocation(combineProgram_, "uBloom");
        combLocIntensity_   = glGetUniformLocation(combineProgram_, "uIntensity");

        createBlurResources(width / 2, height / 2);
    }

    void BloomEffect::resize(int width, int height) {
        int hw = width / 2;
        int hh = height / 2;

        if (hw == halfWidth_ && hh == halfHeight_) {
            return;
        }

        destroyResources();
        createBlurResources(hw, hh);
    }

    void BloomEffect::apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) {
        int hw = width / 2;
        int hh = height / 2;

        // 1) Extract bright pixels to half-res FBO
        glBindFramebuffer(GL_FRAMEBUFFER, brightFBO_);
        glViewport(0, 0, hw, hh);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glUseProgram(brightProgram_);
        glBindTextureUnit(0, inputTexture);
        glUniform1i(brightLocTexture_, 0);
        glUniform1f(brightLocThreshold_, threshold_);
        drawFullscreenQuad();

        // 2) Gaussian blur — ping-pong horizontal/vertical
        uint32_t currentTex = brightTexture_;

        for (int i = 0; i < iterations_; ++i) {
            // Horizontal blur
            glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs_[0]);
            glViewport(0, 0, hw, hh);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(blurHProgram_);
            glBindTextureUnit(0, currentTex);
            glUniform1i(blurHLocTexture_, 0);
            glUniform1f(blurHLocTexelSize_, 1.0f / static_cast<float>(hw));
            drawFullscreenQuad();

            // Vertical blur
            glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs_[1]);
            glViewport(0, 0, hw, hh);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(blurVProgram_);
            glBindTextureUnit(0, blurTextures_[0]);
            glUniform1i(blurVLocTexture_, 0);
            glUniform1f(blurVLocTexelSize_, 1.0f / static_cast<float>(hh));
            drawFullscreenQuad();

            currentTex = blurTextures_[1];
        }

        // 3) Combine: additive blend bloom + original scene
        glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(combineProgram_);
        glBindTextureUnit(0, inputTexture);
        glBindTextureUnit(1, blurTextures_[1]);
        glUniform1i(combLocScene_, 0);
        glUniform1i(combLocBloom_, 1);
        glUniform1f(combLocIntensity_, intensity_);
        drawFullscreenQuad();
    }

    void BloomEffect::createBlurResources(int halfWidth, int halfHeight) {
        halfWidth_  = halfWidth;
        halfHeight_ = halfHeight;

        // Brightness extraction FBO
        glCreateFramebuffers(1, &brightFBO_);
        glCreateTextures(GL_TEXTURE_2D, 1, &brightTexture_);
        glTextureStorage2D(brightTexture_, 1, GL_RGBA16F, halfWidth, halfHeight);
        glTextureParameteri(brightTexture_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(brightTexture_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(brightTexture_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(brightTexture_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glNamedFramebufferTexture(brightFBO_, GL_COLOR_ATTACHMENT0, brightTexture_, 0);

        // Blur ping-pong FBOs
        glCreateFramebuffers(2, blurFBOs_);
        glCreateTextures(GL_TEXTURE_2D, 2, blurTextures_);

        for (int i = 0; i < 2; ++i) {
            glTextureStorage2D(blurTextures_[i], 1, GL_RGBA16F, halfWidth, halfHeight);
            glTextureParameteri(blurTextures_[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(blurTextures_[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(blurTextures_[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(blurTextures_[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glNamedFramebufferTexture(blurFBOs_[i], GL_COLOR_ATTACHMENT0, blurTextures_[i], 0);
        }
    }

    void BloomEffect::destroyResources() {
        if (brightFBO_ != 0) {
            glDeleteFramebuffers(1, &brightFBO_);
            glDeleteTextures(1, &brightTexture_);
            brightFBO_ = 0;
            brightTexture_ = 0;
        }

        if (blurFBOs_[0] != 0) {
            glDeleteFramebuffers(2, blurFBOs_);
            glDeleteTextures(2, blurTextures_);
            blurFBOs_[0] = 0;
            blurFBOs_[1] = 0;
            blurTextures_[0] = 0;
            blurTextures_[1] = 0;
        }

        if (brightProgram_ != 0) {
            glDeleteProgram(brightProgram_);
            brightProgram_ = 0;
        }
        if (blurHProgram_ != 0) {
            glDeleteProgram(blurHProgram_);
            blurHProgram_ = 0;
        }
        if (blurVProgram_ != 0) {
            glDeleteProgram(blurVProgram_);
            blurVProgram_ = 0;
        }
        if (combineProgram_ != 0) {
            glDeleteProgram(combineProgram_);
            combineProgram_ = 0;
        }
    }

} // namespace Nox
