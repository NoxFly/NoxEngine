// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/effects/ChromaticAberrationEffect.hpp>

#include <GL/glew.h>

namespace Nox {

    static constexpr std::string_view ChromaticAberrationFS = R"glsl(
    #version 460 core
    in vec2 vUV;
    uniform sampler2D uInput;
    uniform float uIntensity;
    out vec4 FragColor;

    void main() {
        vec2 dir = vUV - 0.5;
        vec2 offset = dir * uIntensity;

        float r = texture(uInput, vUV + offset).r;
        float g = texture(uInput, vUV).g;
        float b = texture(uInput, vUV - offset).b;

        FragColor = vec4(r, g, b, 1.0);
    }
    )glsl";

    ChromaticAberrationEffect::~ChromaticAberrationEffect() {
        if (program_ != 0)  { glDeleteProgram(program_); }
        if (fbo_ != 0)      { glDeleteFramebuffers(1, &fbo_); }
        if (colorTex_ != 0) { glDeleteTextures(1, &colorTex_); }
    }

    void ChromaticAberrationEffect::init(int width, int height) {
        program_ = compilePostProcessShader(ChromaticAberrationFS);
        resize(width, height);
    }

    void ChromaticAberrationEffect::resize(int width, int height) {
        if (fbo_ != 0) {
            glDeleteFramebuffers(1, &fbo_);
            glDeleteTextures(1, &colorTex_);
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &colorTex_);
        glTextureStorage2D(colorTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(colorTex_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(colorTex_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glCreateFramebuffers(1, &fbo_);
        glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0, colorTex_, 0);
    }

    void ChromaticAberrationEffect::apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) {
        glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
        glViewport(0, 0, width, height);
        glUseProgram(program_);

        glBindTextureUnit(0, inputTexture);
        glUniform1i(glGetUniformLocation(program_, "uInput"), 0);
        glUniform1f(glGetUniformLocation(program_, "uIntensity"), intensity_);

        drawFullscreenQuad();
    }

} // namespace Nox
