// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/renderer/PostProcessEffect.hpp>

#include <cstdint>

namespace Nox {

    /// Screen Space Reflections (SSR) using ray marching in screen space.
    /// Reads from the G-Buffer (position, normal, albedo) and the lit scene
    /// to produce reflection colors.
    class SSREffect : public PostProcessEffect {
    public:
        SSREffect() = default;
        ~SSREffect() override;

        void init(int width, int height) override;
        void resize(int width, int height) override;
        void apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) override;

        [[nodiscard]] std::string_view name() const override { return "SSR"; }

        /// Set the G-Buffer textures needed for SSR.
        void setGBufferTextures(uint32_t positionTex, uint32_t normalTex, uint32_t depthTex);

        /// Set the projection matrix for ray marching.
        void setProjectionMatrix(const float* projData);

        /// Set the view matrix for reflections.
        void setViewMatrix(const float* viewData);

        void setMaxSteps(int steps) { maxSteps_ = steps; }
        void setStepSize(float size) { stepSize_ = size; }
        void setMaxDistance(float dist) { maxDistance_ = dist; }
        void setThickness(float t) { thickness_ = t; }

        [[nodiscard]] int maxSteps() const { return maxSteps_; }
        [[nodiscard]] float stepSize() const { return stepSize_; }
        [[nodiscard]] float maxDistance() const { return maxDistance_; }
        [[nodiscard]] float thickness() const { return thickness_; }

    private:
        uint32_t ssrProgram_    = 0;
        uint32_t ssrFBO_        = 0;
        uint32_t ssrColorTex_   = 0;

        uint32_t gPositionTex_  = 0;
        uint32_t gNormalTex_    = 0;
        uint32_t gDepthTex_     = 0;

        int   maxSteps_    = 64;
        float stepSize_    = 0.05f;
        float maxDistance_  = 50.0f;
        float thickness_   = 0.5f;
    };

} // namespace Nox
