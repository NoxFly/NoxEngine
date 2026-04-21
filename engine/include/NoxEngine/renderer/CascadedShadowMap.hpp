// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <array>
#include <cstdint>

namespace Nox {

    /// Cascaded Shadow Maps (CSM) for large exterior scenes.
    /// Splits the view frustum into N cascades and renders a separate
    /// shadow map for each cascade at increasing resolution.
    class CascadedShadowMap {
    public:
        static constexpr int MaxCascades = 4;

        struct Config {
            int   numCascades     = 4;
            int   shadowMapSize   = 2048;        ///< Resolution per cascade
            float cascadeSplitLambda = 0.75f;    ///< Log/linear split blend factor
            float maxShadowDistance  = 200.0f;    ///< Max shadow distance from camera
        };

        CascadedShadowMap() = default;
        ~CascadedShadowMap();

        CascadedShadowMap(const CascadedShadowMap&) = delete;
        CascadedShadowMap& operator=(const CascadedShadowMap&) = delete;
        CascadedShadowMap(CascadedShadowMap&&) = default;
        CascadedShadowMap& operator=(CascadedShadowMap&&) = default;

        /// Initialize cascade FBOs and textures.
        void init(const Config& config = {});

        /// Compute cascade split distances and light-space matrices.
        void update(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix,
                    float near, float far,
                    const Math::Vec3& lightDirection);

        /// Bind cascade FBO for rendering shadow map at given cascade index.
        void bindCascade(int cascadeIndex) const;

        /// Bind all cascade textures for sampling in the lighting shader.
        /// Binds to texture units startUnit..startUnit+numCascades-1.
        void bindTextures(int startUnit = 5) const;

        /// Unbind (restore default FBO).
        static void unbind();

        [[nodiscard]] const Config& config() const { return config_; }
        [[nodiscard]] int numCascades() const { return config_.numCascades; }

        /// Get the light-space matrix for a specific cascade.
        [[nodiscard]] const Math::Mat4& lightSpaceMatrix(int cascade) const {
            return lightSpaceMatrices_[cascade];
        }

        /// Get all cascade split distances (in view space).
        [[nodiscard]] const std::array<float, MaxCascades + 1>& cascadeSplits() const {
            return cascadeSplits_;
        }

        [[nodiscard]] bool isInitialized() const { return initialized_; }

    private:
        void destroy();
        void computeCascadeSplits(float near, float far);

        Config   config_;
        uint32_t fbo_ = 0;
        std::array<uint32_t, MaxCascades> depthTextures_{};
        std::array<Math::Mat4, MaxCascades> lightSpaceMatrices_{};
        std::array<float, MaxCascades + 1> cascadeSplits_{};
        bool initialized_ = false;
    };

} // namespace Nox
