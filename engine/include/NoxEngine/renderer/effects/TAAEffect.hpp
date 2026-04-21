// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/renderer/PostProcessEffect.hpp>

#include <cstdint>

namespace Nox {

    /// Temporal Anti-Aliasing (TAA).
    /// Uses jittered projection + history buffer to reduce aliasing over time.
    /// Requires velocity buffer for motion rejection.
    class TAAEffect : public PostProcessEffect {
    public:
        TAAEffect() = default;
        ~TAAEffect() override;

        void init(int width, int height) override;
        void resize(int width, int height) override;
        void apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) override;

        [[nodiscard]] std::string_view name() const override { return "TAA"; }

        /// Set the depth texture for depth-based rejection.
        void setDepthTexture(uint32_t depthTex);

        /// Set velocity/motion texture for motion rejection.
        void setVelocityTexture(uint32_t velocityTex);

        /// Get the current jitter offset (to apply to projection matrix).
        /// Returns (x, y) in pixel-fraction units.
        [[nodiscard]] const float* jitterOffset() const { return jitter_; }

        /// Advance to the next jitter sample.
        void nextFrame();

        void setBlendFactor(float f) { blendFactor_ = f; }
        [[nodiscard]] float blendFactor() const { return blendFactor_; }

    private:
        uint32_t taaProgram_     = 0;
        uint32_t historyTex_     = 0;
        uint32_t historyFBO_     = 0;
        uint32_t prevHistoryTex_ = 0;
        uint32_t depthTex_       = 0;
        uint32_t velocityTex_    = 0;

        float    jitter_[2]      = { 0.0f, 0.0f };
        int      sampleIndex_    = 0;
        float    blendFactor_    = 0.9f;  ///< Blend towards history (0 = no TAA, 1 = full)
    };

} // namespace Nox
