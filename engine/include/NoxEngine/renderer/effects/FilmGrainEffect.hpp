// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/renderer/PostProcessEffect.hpp>

#include <cstdint>

namespace Nox {

    /// Adds a subtle film grain noise overlay on the final image.
    class FilmGrainEffect : public PostProcessEffect {
    public:
        FilmGrainEffect() = default;
        ~FilmGrainEffect() override;

        void init(int width, int height) override;
        void resize(int width, int height) override;
        void apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) override;

        [[nodiscard]] std::string_view name() const override { return "Film Grain"; }

        void setIntensity(float i) { intensity_ = i; }
        [[nodiscard]] float intensity() const { return intensity_; }

    private:
        uint32_t program_ = 0;
        uint32_t fbo_     = 0;
        uint32_t colorTex_ = 0;
        float    intensity_ = 0.08f;
        float    time_      = 0.0f;
    };

} // namespace Nox
