// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/effects/FXAAEffect.hpp>

#include <GL/glew.h>

namespace Nox {

    FXAAEffect::~FXAAEffect() {
        if (program_ != 0) {
            glDeleteProgram(program_);
        }
    }

    void FXAAEffect::init([[maybe_unused]] int width, [[maybe_unused]] int height) {
        program_ = compilePostProcessShader(FragmentSource);
        locTexture_ = glGetUniformLocation(program_, "uTexture");
        locInverseScreenSize_ = glGetUniformLocation(program_, "uInverseScreenSize");
    }

    void FXAAEffect::resize([[maybe_unused]] int width, [[maybe_unused]] int height) {
        // No internal resources to resize
    }

    void FXAAEffect::apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) {
        glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glUseProgram(program_);
        glBindTextureUnit(0, inputTexture);
        glUniform1i(locTexture_, 0);
        glUniform2f(locInverseScreenSize_, 1.0f / static_cast<float>(width),
                    1.0f / static_cast<float>(height));

        drawFullscreenQuad();
    }

} // namespace Nox
