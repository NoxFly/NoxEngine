// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/effects/VignetteEffect.hpp>

#include <GL/glew.h>

namespace Nox {

    static constexpr std::string_view VignetteFS = R"glsl(
    #version 460 core
    in vec2 vUV;
    uniform sampler2D uInput;
    uniform float uIntensity;
    uniform float uSmoothness;
    out vec4 FragColor;

    void main() {
        vec3 color = texture(uInput, vUV).rgb;
        vec2 center = vUV - 0.5;
        float dist = length(center);
        float vignette = smoothstep(0.5, 0.5 - uSmoothness, dist * (1.0 + uIntensity));
        color *= vignette;
        FragColor = vec4(color, 1.0);
    }
    )glsl";

    VignetteEffect::~VignetteEffect() {
        if (program_ != 0)  { glDeleteProgram(program_); }
        if (fbo_ != 0)      { glDeleteFramebuffers(1, &fbo_); }
        if (colorTex_ != 0) { glDeleteTextures(1, &colorTex_); }
    }

    void VignetteEffect::init(int width, int height) {
        program_ = compilePostProcessShader(VignetteFS);
        resize(width, height);
    }

    void VignetteEffect::resize(int width, int height) {
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

    void VignetteEffect::apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) {
        glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
        glViewport(0, 0, width, height);
        glUseProgram(program_);

        glBindTextureUnit(0, inputTexture);
        glUniform1i(glGetUniformLocation(program_, "uInput"), 0);
        glUniform1f(glGetUniformLocation(program_, "uIntensity"), intensity_);
        glUniform1f(glGetUniformLocation(program_, "uSmoothness"), smoothness_);

        drawFullscreenQuad();
    }

} // namespace Nox
