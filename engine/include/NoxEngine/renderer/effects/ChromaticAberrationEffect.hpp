// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/renderer/PostProcessEffect.hpp>

#include <cstdint>

namespace Nox {

    /// Separates RGB color channels to simulate lens fringing.
    class ChromaticAberrationEffect : public PostProcessEffect {
    public:
        ChromaticAberrationEffect() = default;
        ~ChromaticAberrationEffect() override;

        void init(int width, int height) override;
        void resize(int width, int height) override;
        void apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) override;

        [[nodiscard]] std::string_view name() const override { return "Chromatic Aberration"; }

        void setIntensity(float i) { intensity_ = i; }
        [[nodiscard]] float intensity() const { return intensity_; }

    private:
        uint32_t program_  = 0;
        uint32_t fbo_      = 0;
        uint32_t colorTex_ = 0;
        float    intensity_ = 0.003f;
    };

} // namespace Nox
