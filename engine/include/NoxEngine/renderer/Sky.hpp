// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>

namespace Nox {

    /// Physically-based sky model using Preetham analytic sky model
    /// with Rayleigh + Mie atmospheric scattering.
    class Sky {
    public:
        Sky();
        ~Sky();

        Sky(const Sky&) = delete;
        Sky& operator=(const Sky&) = delete;
        Sky(Sky&&) = default;
        Sky& operator=(Sky&&) = default;

        /// Set the sun direction (normalized, pointing toward the sun).
        void setSunDirection(const Math::Vec3& dir);

        /// Set turbidity (2 = clear, 10 = hazy). Affects scattering intensity.
        void setTurbidity(float turbidity);

        /// Set ground albedo (reflectivity of the ground plane).
        void setGroundAlbedo(float albedo);

        /// Set sun intensity multiplier.
        void setSunIntensity(float intensity);

        /// Set Rayleigh scattering coefficient.
        void setRayleighCoefficient(float coeff);

        /// Set Mie scattering coefficient.
        void setMieCoefficient(float coeff);

        /// Set Mie preferred scattering direction (g parameter, -1 to 1).
        void setMieDirectionalG(float g);

        [[nodiscard]] const Math::Vec3& sunDirection() const { return sunDirection_; }
        [[nodiscard]] float turbidity() const { return turbidity_; }
        [[nodiscard]] float groundAlbedo() const { return groundAlbedo_; }
        [[nodiscard]] float sunIntensity() const { return sunIntensity_; }
        [[nodiscard]] float rayleighCoefficient() const { return rayleighCoefficient_; }
        [[nodiscard]] float mieCoefficient() const { return mieCoefficient_; }
        [[nodiscard]] float mieDirectionalG() const { return mieDirectionalG_; }

        /// Render the sky dome. Called by Engine internally.
        void render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix,
                    const Math::Vec3& cameraPos);

        /// Initialize GPU resources.
        void init();

        /// Whether GPU resources are ready.
        [[nodiscard]] bool isInitialized() const { return initialized_; }

    private:
        void createSkyDome();
        void compileSkyShader();

        Math::Vec3 sunDirection_{ 0.0f, 0.8f, -0.6f };
        float turbidity_          = 2.0f;
        float groundAlbedo_       = 0.3f;
        float sunIntensity_       = 22.0f;
        float rayleighCoefficient_ = 2.0f;
        float mieCoefficient_     = 0.005f;
        float mieDirectionalG_    = 0.8f;

        uint32_t skyVAO_       = 0;
        uint32_t skyVBO_       = 0;
        uint32_t skyIBO_       = 0;
        uint32_t skyProgram_   = 0;
        uint32_t skyIndexCount_ = 0;
        bool     initialized_  = false;
    };

} // namespace Nox
