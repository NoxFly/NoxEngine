// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/renderer/PostProcessEffect.hpp>

#include <cstdint>

namespace Nox {

    /// Volumetric fog / god rays using ray marching.
    /// Samples the depth buffer and shadow maps to compute in-scattered light.
    class VolumetricFogEffect : public PostProcessEffect {
    public:
        VolumetricFogEffect() = default;
        ~VolumetricFogEffect() override;

        void init(int width, int height) override;
        void resize(int width, int height) override;
        void apply(uint32_t inputTexture, uint32_t outputFBO, int width, int height) override;

        [[nodiscard]] std::string_view name() const override { return "Volumetric Fog"; }

        /// Set required textures.
        void setDepthTexture(uint32_t depthTex);
        void setShadowMapTexture(uint32_t shadowTex);

        /// Set the light direction for god rays.
        void setLightDirection(const float* dir);

        /// Set the light-space matrix for shadow sampling.
        void setLightSpaceMatrix(const float* mat);

        /// Set camera matrices.
        void setViewMatrix(const float* viewData);
        void setProjectionMatrix(const float* projData);
        void setCameraPosition(const float* pos);

        void setDensity(float d) { density_ = d; }
        void setScattering(float s) { scattering_ = s; }
        void setNumSteps(int n) { numSteps_ = n; }
        void setFogColor(float r, float g, float b) {
            fogColor_[0] = r; fogColor_[1] = g; fogColor_[2] = b;
        }

        [[nodiscard]] float density() const { return density_; }
        [[nodiscard]] float scattering() const { return scattering_; }
        [[nodiscard]] int numSteps() const { return numSteps_; }

    private:
        uint32_t fogProgram_   = 0;
        uint32_t fogFBO_       = 0;
        uint32_t fogColorTex_  = 0;
        uint32_t depthTex_     = 0;
        uint32_t shadowTex_    = 0;

        float    density_      = 0.02f;
        float    scattering_   = 0.7f;
        int      numSteps_     = 32;
        float    fogColor_[3]  = { 0.5f, 0.6f, 0.7f };

        float    lightDir_[3]       = { 0.0f, -1.0f, 0.0f };
        float    lightSpaceMat_[16] = {};
        float    viewMat_[16]       = {};
        float    projMat_[16]       = {};
        float    cameraPos_[3]      = {};
    };

} // namespace Nox
