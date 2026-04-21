// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/CascadedShadowMap.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

namespace Nox {

    CascadedShadowMap::~CascadedShadowMap() {
        destroy();
    }

    void CascadedShadowMap::init(const Config& config) {
        if (initialized_) {
            destroy();
        }

        config_ = config;

        // Create a single FBO, we'll switch depth attachments per cascade
        glCreateFramebuffers(1, &fbo_);

        // Create depth textures for each cascade
        for (int i = 0; i < config_.numCascades; ++i) {
            glCreateTextures(GL_TEXTURE_2D, 1, &depthTextures_[i]);
            glTextureStorage2D(depthTextures_[i], 1, GL_DEPTH_COMPONENT32F,
                               config_.shadowMapSize, config_.shadowMapSize);
            glTextureParameteri(depthTextures_[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(depthTextures_[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(depthTextures_[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(depthTextures_[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTextureParameterfv(depthTextures_[i], GL_TEXTURE_BORDER_COLOR, borderColor);
            glTextureParameteri(depthTextures_[i], GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTextureParameteri(depthTextures_[i], GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        }

        // Attach the first cascade by default
        glNamedFramebufferTexture(fbo_, GL_DEPTH_ATTACHMENT, depthTextures_[0], 0);
        glNamedFramebufferDrawBuffer(fbo_, GL_NONE);
        glNamedFramebufferReadBuffer(fbo_, GL_NONE);

        GLenum status = glCheckNamedFramebufferStatus(fbo_, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            NOX_LOG_ERROR("CSM FBO incomplete: 0x{:X}", status);
            destroy();
            return;
        }

        initialized_ = true;
        NOX_LOG_INFO("CascadedShadowMap initialized ({} cascades, {}px)",
                     config_.numCascades, config_.shadowMapSize);
    }

    void CascadedShadowMap::update(const Math::Mat4& viewMatrix,
                                    const Math::Mat4& projMatrix,
                                    float near, float far,
                                    const Math::Vec3& lightDirection) {
        if (!initialized_) { return; }

        computeCascadeSplits(near, far);

        Math::Mat4 invViewProj = glm::inverse(projMatrix * viewMatrix);

        for (int i = 0; i < config_.numCascades; ++i) {
            float cascadeNear = cascadeSplits_[i];
            float cascadeFar  = cascadeSplits_[i + 1];

            // Compute frustum corners for this cascade
            Math::Vec3 frustumCorners[8] = {
                // Near plane
                Math::Vec3(-1, -1, -1), Math::Vec3( 1, -1, -1),
                Math::Vec3( 1,  1, -1), Math::Vec3(-1,  1, -1),
                // Far plane
                Math::Vec3(-1, -1,  1), Math::Vec3( 1, -1,  1),
                Math::Vec3( 1,  1,  1), Math::Vec3(-1,  1,  1),
            };

            // Build sub-frustum projection
            // We need to compute the sub-frustum near/far in NDC
            [[maybe_unused]] float nearNDC = 2.0f * (cascadeNear - near) / (far - near) - 1.0f;
            [[maybe_unused]] float farNDC  = 2.0f * (cascadeFar  - near) / (far - near) - 1.0f;

            // Transform frustum corners to world space
            for (auto& corner : frustumCorners) {
                Math::Vec4 worldPos = invViewProj * Math::Vec4(corner, 1.0f);
                corner = Math::Vec3(worldPos) / worldPos.w;
            }

            // Linearly interpolate between the full frustum near/far based on cascade split
            Math::Vec3 cascadeCorners[8];
            for (int j = 0; j < 4; ++j) {
                Math::Vec3 nearCorner = frustumCorners[j];
                Math::Vec3 farCorner = frustumCorners[j + 4];
                float nearT = (cascadeNear - near) / (far - near);
                float farT = (cascadeFar - near) / (far - near);
                cascadeCorners[j] = nearCorner + (farCorner - nearCorner) * nearT;
                cascadeCorners[j + 4] = nearCorner + (farCorner - nearCorner) * farT;
            }

            // Find center of the cascade frustum
            Math::Vec3 center(0.0f);
            for (const auto& c : cascadeCorners) {
                center += c;
            }
            center /= 8.0f;

            // Compute the radius of the bounding sphere
            float radius = 0.0f;
            for (const auto& c : cascadeCorners) {
                float dist = glm::length(c - center);
                radius = std::max(radius, dist);
            }

            // Round to texel size to reduce shimmering
            radius = std::ceil(radius * 16.0f) / 16.0f;

            Math::Vec3 lightDir = glm::normalize(lightDirection);
            Math::Mat4 lightView = glm::lookAt(
                center - lightDir * radius,
                center,
                Math::Vec3(0.0f, 1.0f, 0.0f)
            );

            Math::Mat4 lightProj = glm::ortho(
                -radius, radius,
                -radius, radius,
                0.0f, 2.0f * radius
            );

            lightSpaceMatrices_[i] = lightProj * lightView;
        }
    }

    void CascadedShadowMap::bindCascade(int cascadeIndex) const {
        if (!initialized_ || cascadeIndex < 0 || cascadeIndex >= config_.numCascades) {
            return;
        }

        glNamedFramebufferTexture(fbo_, GL_DEPTH_ATTACHMENT, depthTextures_[cascadeIndex], 0);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glViewport(0, 0, config_.shadowMapSize, config_.shadowMapSize);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void CascadedShadowMap::bindTextures(int startUnit) const {
        for (int i = 0; i < config_.numCascades; ++i) {
            glBindTextureUnit(startUnit + i, depthTextures_[i]);
        }
    }

    void CascadedShadowMap::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void CascadedShadowMap::computeCascadeSplits(float near, float far) {
        float lambda = config_.cascadeSplitLambda;
        float range = far - near;
        float ratio = far / near;

        cascadeSplits_[0] = near;

        for (int i = 1; i <= config_.numCascades; ++i) {
            float p = static_cast<float>(i) / static_cast<float>(config_.numCascades);

            float logSplit = near * std::pow(ratio, p);
            float linearSplit = near + range * p;

            float split = lambda * logSplit + (1.0f - lambda) * linearSplit;
            split = std::min(split, config_.maxShadowDistance);

            cascadeSplits_[i] = split;
        }
    }

    void CascadedShadowMap::destroy() {
        if (fbo_ != 0) {
            glDeleteFramebuffers(1, &fbo_);
            fbo_ = 0;
        }
        for (int i = 0; i < MaxCascades; ++i) {
            if (depthTextures_[i] != 0) {
                glDeleteTextures(1, &depthTextures_[i]);
                depthTextures_[i] = 0;
            }
        }
        initialized_ = false;
    }

} // namespace Nox
