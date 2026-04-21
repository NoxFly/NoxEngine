// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/effects/SSREffect.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#include <cstring>

namespace Nox {

    static constexpr std::string_view SSRFragmentSource = R"glsl(
    #version 460 core

    in vec2 vUV;
    out vec4 FragColor;

    uniform sampler2D uSceneColor;
    uniform sampler2D uGPosition;
    uniform sampler2D uGNormal;
    uniform sampler2D uGDepth;

    uniform mat4 uProjection;
    uniform mat4 uView;
    uniform int  uMaxSteps;
    uniform float uStepSize;
    uniform float uMaxDistance;
    uniform float uThickness;

    vec3 viewSpacePosition(vec2 uv) {
        return texture(uGPosition, uv).xyz;
    }

    void main() {
        vec3 fragPos = texture(uGPosition, vUV).xyz;
        vec3 normal  = normalize(texture(uGNormal, vUV).xyz);
        float roughness = texture(uGNormal, vUV).w;

        // Skip highly rough surfaces — no visible reflections
        if (roughness > 0.7) {
            FragColor = texture(uSceneColor, vUV);
            return;
        }

        vec3 viewPos = (uView * vec4(fragPos, 1.0)).xyz;
        vec3 viewNormal = normalize((uView * vec4(normal, 0.0)).xyz);
        vec3 viewDir = normalize(viewPos);
        vec3 reflDir = reflect(viewDir, viewNormal);

        // Ray march in view space
        vec3 rayPos = viewPos;
        vec3 rayDir = reflDir * uStepSize;

        vec4 sceneColor = texture(uSceneColor, vUV);
        vec4 reflColor = vec4(0.0);
        float reflStrength = 0.0;

        for (int i = 0; i < uMaxSteps; ++i) {
            rayPos += rayDir;

            // Check distance
            if (length(rayPos - viewPos) > uMaxDistance) break;

            // Project to screen space
            vec4 clipPos = uProjection * vec4(rayPos, 1.0);
            vec2 screenUV = (clipPos.xy / clipPos.w) * 0.5 + 0.5;

            // Check bounds
            if (screenUV.x < 0.0 || screenUV.x > 1.0 ||
                screenUV.y < 0.0 || screenUV.y > 1.0) break;

            // Sample the depth at this screen position
            vec3 samplePos = (uView * vec4(texture(uGPosition, screenUV).xyz, 1.0)).xyz;
            float depthDiff = rayPos.z - samplePos.z;

            if (depthDiff > 0.0 && depthDiff < uThickness) {
                reflColor = texture(uSceneColor, screenUV);
                reflStrength = 1.0 - roughness;

                // Fade at edges
                vec2 edgeFade = smoothstep(vec2(0.0), vec2(0.1), screenUV)
                              * (1.0 - smoothstep(vec2(0.9), vec2(1.0), screenUV));
                reflStrength *= edgeFade.x * edgeFade.y;

                // Fade with distance
                float distFade = 1.0 - float(i) / float(uMaxSteps);
                reflStrength *= distFade;

                break;
            }
        }

        FragColor = mix(sceneColor, reflColor, reflStrength * 0.5);
    }
    )glsl";

    SSREffect::~SSREffect() {
        if (ssrProgram_ != 0)  { glDeleteProgram(ssrProgram_); }
        if (ssrFBO_ != 0)      { glDeleteFramebuffers(1, &ssrFBO_); }
        if (ssrColorTex_ != 0) { glDeleteTextures(1, &ssrColorTex_); }
    }

    void SSREffect::init(int width, int height) {
        ssrProgram_ = compilePostProcessShader(SSRFragmentSource);
        if (ssrProgram_ == 0) {
            NOX_LOG_ERROR("Failed to compile SSR shader");
            return;
        }

        // Create FBO and color texture
        glCreateFramebuffers(1, &ssrFBO_);
        glCreateTextures(GL_TEXTURE_2D, 1, &ssrColorTex_);
        glTextureStorage2D(ssrColorTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(ssrColorTex_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(ssrColorTex_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glNamedFramebufferTexture(ssrFBO_, GL_COLOR_ATTACHMENT0, ssrColorTex_, 0);
    }

    void SSREffect::resize(int width, int height) {
        if (ssrColorTex_ != 0) { glDeleteTextures(1, &ssrColorTex_); }
        if (ssrFBO_ != 0) { glDeleteFramebuffers(1, &ssrFBO_); }

        glCreateFramebuffers(1, &ssrFBO_);
        glCreateTextures(GL_TEXTURE_2D, 1, &ssrColorTex_);
        glTextureStorage2D(ssrColorTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(ssrColorTex_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(ssrColorTex_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glNamedFramebufferTexture(ssrFBO_, GL_COLOR_ATTACHMENT0, ssrColorTex_, 0);
    }

    void SSREffect::apply(uint32_t inputTexture, uint32_t outputFBO,
                           int width, int height) {
        if (ssrProgram_ == 0) { return; }

        glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
        glViewport(0, 0, width, height);

        glUseProgram(ssrProgram_);
        glBindTextureUnit(0, inputTexture);
        glUniform1i(glGetUniformLocation(ssrProgram_, "uSceneColor"), 0);

        glBindTextureUnit(1, gPositionTex_);
        glUniform1i(glGetUniformLocation(ssrProgram_, "uGPosition"), 1);

        glBindTextureUnit(2, gNormalTex_);
        glUniform1i(glGetUniformLocation(ssrProgram_, "uGNormal"), 2);

        glBindTextureUnit(3, gDepthTex_);
        glUniform1i(glGetUniformLocation(ssrProgram_, "uGDepth"), 3);

        glUniform1i(glGetUniformLocation(ssrProgram_, "uMaxSteps"), maxSteps_);
        glUniform1f(glGetUniformLocation(ssrProgram_, "uStepSize"), stepSize_);
        glUniform1f(glGetUniformLocation(ssrProgram_, "uMaxDistance"), maxDistance_);
        glUniform1f(glGetUniformLocation(ssrProgram_, "uThickness"), thickness_);

        drawFullscreenQuad();
    }

    void SSREffect::setGBufferTextures(uint32_t positionTex, uint32_t normalTex,
                                        uint32_t depthTex) {
        gPositionTex_ = positionTex;
        gNormalTex_   = normalTex;
        gDepthTex_    = depthTex;
    }

    void SSREffect::setProjectionMatrix(const float* projData) {
        if (ssrProgram_ != 0) {
            glUseProgram(ssrProgram_);
            glUniformMatrix4fv(glGetUniformLocation(ssrProgram_, "uProjection"),
                               1, GL_FALSE, projData);
        }
    }

    void SSREffect::setViewMatrix(const float* viewData) {
        if (ssrProgram_ != 0) {
            glUseProgram(ssrProgram_);
            glUniformMatrix4fv(glGetUniformLocation(ssrProgram_, "uView"),
                               1, GL_FALSE, viewData);
        }
    }

} // namespace Nox
