// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/effects/VolumetricFogEffect.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#include <cstring>

namespace Nox {

    static constexpr std::string_view VolumetricFogFragmentSource = R"glsl(
    #version 460 core

    in vec2 vUV;
    out vec4 FragColor;

    uniform sampler2D uSceneColor;
    uniform sampler2D uDepthTexture;
    uniform sampler2DShadow uShadowMap;

    uniform mat4 uInvViewProj;
    uniform mat4 uLightSpaceMatrix;
    uniform vec3 uLightDirection;
    uniform vec3 uCameraPosition;
    uniform vec3 uFogColor;
    uniform float uDensity;
    uniform float uScattering;
    uniform int uNumSteps;

    // Henyey-Greenstein phase function
    float henyeyGreenstein(float cosTheta, float g) {
        float g2 = g * g;
        float denom = 1.0 + g2 - 2.0 * g * cosTheta;
        return (1.0 - g2) / (4.0 * 3.14159265 * pow(max(denom, 0.0001), 1.5));
    }

    vec3 worldPosFromDepth(vec2 uv, float depth) {
        vec4 clipPos = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
        vec4 worldPos = uInvViewProj * clipPos;
        return worldPos.xyz / worldPos.w;
    }

    float shadowLookup(vec3 worldPos) {
        vec4 lightSpacePos = uLightSpaceMatrix * vec4(worldPos, 1.0);
        vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
        projCoords = projCoords * 0.5 + 0.5;

        if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
            projCoords.y < 0.0 || projCoords.y > 1.0 ||
            projCoords.z > 1.0) {
            return 1.0;
        }

        return texture(uShadowMap, vec3(projCoords.xy, projCoords.z));
    }

    void main() {
        vec4 sceneColor = texture(uSceneColor, vUV);
        float depth = texture(uDepthTexture, vUV).r;

        if (depth >= 1.0) {
            // Sky — no fog
            FragColor = sceneColor;
            return;
        }

        vec3 worldPos = worldPosFromDepth(vUV, depth);
        vec3 rayDir = normalize(worldPos - uCameraPosition);
        float totalDistance = length(worldPos - uCameraPosition);
        float stepSize = totalDistance / float(uNumSteps);

        // Ray march from camera to fragment
        float accumulated = 0.0;
        float transmittance = 1.0;
        vec3 fogContrib = vec3(0.0);

        float cosTheta = dot(rayDir, -normalize(uLightDirection));
        float phase = henyeyGreenstein(cosTheta, uScattering);

        for (int i = 0; i < uNumSteps; ++i) {
            float t = (float(i) + 0.5) * stepSize;
            vec3 samplePos = uCameraPosition + rayDir * t;

            // Height-based density falloff
            float heightFalloff = exp(-max(samplePos.y, 0.0) * 0.1);
            float localDensity = uDensity * heightFalloff;

            // Shadow test
            float shadow = shadowLookup(samplePos);

            // Accumulate in-scattering
            float scatterAmount = localDensity * stepSize;
            fogContrib += transmittance * scatterAmount * phase * shadow * uFogColor;
            transmittance *= exp(-scatterAmount);

            if (transmittance < 0.01) break;
        }

        vec3 result = sceneColor.rgb * transmittance + fogContrib;
        FragColor = vec4(result, sceneColor.a);
    }
    )glsl";

    VolumetricFogEffect::~VolumetricFogEffect() {
        if (fogProgram_ != 0)  { glDeleteProgram(fogProgram_); }
        if (fogFBO_ != 0)      { glDeleteFramebuffers(1, &fogFBO_); }
        if (fogColorTex_ != 0) { glDeleteTextures(1, &fogColorTex_); }
    }

    void VolumetricFogEffect::init(int width, int height) {
        fogProgram_ = compilePostProcessShader(VolumetricFogFragmentSource);
        if (fogProgram_ == 0) {
            NOX_LOG_ERROR("Failed to compile volumetric fog shader");
            return;
        }

        glCreateFramebuffers(1, &fogFBO_);
        glCreateTextures(GL_TEXTURE_2D, 1, &fogColorTex_);
        glTextureStorage2D(fogColorTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(fogColorTex_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(fogColorTex_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glNamedFramebufferTexture(fogFBO_, GL_COLOR_ATTACHMENT0, fogColorTex_, 0);
    }

    void VolumetricFogEffect::resize(int width, int height) {
        if (fogColorTex_ != 0) { glDeleteTextures(1, &fogColorTex_); }
        if (fogFBO_ != 0)      { glDeleteFramebuffers(1, &fogFBO_); }

        glCreateFramebuffers(1, &fogFBO_);
        glCreateTextures(GL_TEXTURE_2D, 1, &fogColorTex_);
        glTextureStorage2D(fogColorTex_, 1, GL_RGBA16F, width, height);
        glTextureParameteri(fogColorTex_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(fogColorTex_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glNamedFramebufferTexture(fogFBO_, GL_COLOR_ATTACHMENT0, fogColorTex_, 0);
    }

    void VolumetricFogEffect::apply(uint32_t inputTexture, uint32_t outputFBO,
                                     int width, int height) {
        if (fogProgram_ == 0) { return; }

        glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
        glViewport(0, 0, width, height);

        glUseProgram(fogProgram_);

        glBindTextureUnit(0, inputTexture);
        glUniform1i(glGetUniformLocation(fogProgram_, "uSceneColor"), 0);

        if (depthTex_ != 0) {
            glBindTextureUnit(1, depthTex_);
            glUniform1i(glGetUniformLocation(fogProgram_, "uDepthTexture"), 1);
        }

        if (shadowTex_ != 0) {
            glBindTextureUnit(2, shadowTex_);
            glUniform1i(glGetUniformLocation(fogProgram_, "uShadowMap"), 2);
        }

        glUniformMatrix4fv(glGetUniformLocation(fogProgram_, "uLightSpaceMatrix"),
                           1, GL_FALSE, lightSpaceMat_);
        glUniform3fv(glGetUniformLocation(fogProgram_, "uLightDirection"), 1, lightDir_);
        glUniform3fv(glGetUniformLocation(fogProgram_, "uCameraPosition"), 1, cameraPos_);
        glUniform3fv(glGetUniformLocation(fogProgram_, "uFogColor"), 1, fogColor_);
        glUniform1f(glGetUniformLocation(fogProgram_, "uDensity"), density_);
        glUniform1f(glGetUniformLocation(fogProgram_, "uScattering"), scattering_);
        glUniform1i(glGetUniformLocation(fogProgram_, "uNumSteps"), numSteps_);

        // Compute inverse view-projection
        // The caller should set this via setViewMatrix/setProjectionMatrix
        glUniformMatrix4fv(glGetUniformLocation(fogProgram_, "uInvViewProj"),
                           1, GL_FALSE, viewMat_);

        drawFullscreenQuad();
    }

    void VolumetricFogEffect::setDepthTexture(uint32_t depthTex) {
        depthTex_ = depthTex;
    }

    void VolumetricFogEffect::setShadowMapTexture(uint32_t shadowTex) {
        shadowTex_ = shadowTex;
    }

    void VolumetricFogEffect::setLightDirection(const float* dir) {
        std::memcpy(lightDir_, dir, sizeof(float) * 3);
    }

    void VolumetricFogEffect::setLightSpaceMatrix(const float* mat) {
        std::memcpy(lightSpaceMat_, mat, sizeof(float) * 16);
    }

    void VolumetricFogEffect::setViewMatrix(const float* viewData) {
        std::memcpy(viewMat_, viewData, sizeof(float) * 16);
    }

    void VolumetricFogEffect::setProjectionMatrix(const float* projData) {
        std::memcpy(projMat_, projData, sizeof(float) * 16);
    }

    void VolumetricFogEffect::setCameraPosition(const float* pos) {
        std::memcpy(cameraPos_, pos, sizeof(float) * 3);
    }

} // namespace Nox
