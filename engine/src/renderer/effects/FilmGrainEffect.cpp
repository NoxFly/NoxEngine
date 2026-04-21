// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/effects/FilmGrainEffect.hpp>

#include <GL/glew.h>

namespace Nox {

    static constexpr std::string_view FilmGrainFS = R"glsl(
    #version 460 core
    in vec2 vUV;
    uniform sampler2D uInput;
    uniform float uIntensity;
    uniform float uTime;
    out vec4 FragColor;

    // Simple hash-based noise
    float noise(vec2 co) {
        return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
    }

    void main() {
        vec3 color = texture(uInput, vUV).rgb;
        float grain = noise(vUV * 1000.0 + uTime) * 2.0 - 1.0;
        color += grain * uIntensity;
        FragColor = vec4(color, 1.0);
    }
    )glsl";

    FilmGrainEffect::~FilmGrainEffect() {
        if (program_ != 0)  { glDeleteProgram(program_); }
        if (fbo_ != 0)      { glDeleteFramebuffers(1, &fbo_); }
        if (colorTex_ != 0) { glDeleteTextures(1, &colorTex_); }
    }

    void FilmGrainEffect::init(int width, int height) {
        program_ = compilePostProcessShader(FilmGrainFS);
        resize(width, height);
    }

    void FilmGrainEffect::resize(int width, int height) {
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

    void FilmGrainEffect::apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) {
        time_ += 0.016f; // approximate dt

        glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
        glViewport(0, 0, width, height);
        glUseProgram(program_);

        glBindTextureUnit(0, inputTexture);
        glUniform1i(glGetUniformLocation(program_, "uInput"), 0);
        glUniform1f(glGetUniformLocation(program_, "uIntensity"), intensity_);
        glUniform1f(glGetUniformLocation(program_, "uTime"), time_);

        drawFullscreenQuad();
    }

} // namespace Nox
