// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/renderer/PostProcessEffect.hpp>

#include <cstdint>

namespace Nox {

    /// Darkens the edges of the screen to draw focus toward the center.
    class VignetteEffect : public PostProcessEffect {
    public:
        VignetteEffect() = default;
        ~VignetteEffect() override;

        void init(int width, int height) override;
        void resize(int width, int height) override;
        void apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) override;

        [[nodiscard]] std::string_view name() const override { return "Vignette"; }

        void setIntensity(float i) { intensity_ = i; }
        void setSmoothness(float s) { smoothness_ = s; }
        [[nodiscard]] float intensity() const { return intensity_; }
        [[nodiscard]] float smoothness() const { return smoothness_; }

    private:
        uint32_t program_  = 0;
        uint32_t fbo_      = 0;
        uint32_t colorTex_ = 0;
        float    intensity_  = 0.8f;
        float    smoothness_ = 0.4f;
    };

} // namespace Nox
