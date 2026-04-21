// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/renderer/ComputeShader.hpp>
#include <NoxEngine/scene/SceneObject.hpp>

#include <cstdint>

namespace Nox {

    /// Compute-driven GPU particle system.
    /// Simulation runs entirely on the GPU via a compute shader.
    /// Supports millions of particles with zero CPU overhead per frame.
    class GPUParticleSystem : public SceneObject {
    public:
        /// Particle emitter configuration.
        struct Config {
            int       maxParticles    = 100000;    ///< Maximum particle count
            float     emitRate        = 10000.0f;  ///< Particles emitted per second
            float     lifeMin         = 1.0f;      ///< Minimum lifetime (seconds)
            float     lifeMax         = 3.0f;      ///< Maximum lifetime (seconds)
            float     speedMin        = 1.0f;      ///< Minimum initial speed
            float     speedMax        = 5.0f;      ///< Maximum initial speed
            float     sizeStart       = 0.05f;     ///< Start size
            float     sizeEnd         = 0.0f;      ///< End size (fades out)
            Math::Vec3 gravity        = { 0.0f, -9.81f, 0.0f };
            Math::Vec3 emitDirection  = { 0.0f, 1.0f, 0.0f };
            float     emitSpread      = 0.5f;      ///< Cone spread (0=tight, 1=hemisphere)
            Math::Vec4 colorStart     = { 1.0f, 1.0f, 1.0f, 1.0f };
            Math::Vec4 colorEnd       = { 1.0f, 1.0f, 1.0f, 0.0f };
        };

        GPUParticleSystem();
        ~GPUParticleSystem() override;

        GPUParticleSystem(const GPUParticleSystem&) = delete;
        GPUParticleSystem& operator=(const GPUParticleSystem&) = delete;
        GPUParticleSystem(GPUParticleSystem&&) = default;
        GPUParticleSystem& operator=(GPUParticleSystem&&) = default;

        /// Initialize GPU resources with the given configuration.
        void init(const Config& config = {});

        /// Update simulation (dispatches compute shader).
        void update(float dt);

        /// Render particles as point sprites.
        void render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix);

        void setConfig(const Config& config) { config_ = config; }
        [[nodiscard]] const Config& config() const { return config_; }
        [[nodiscard]] int activeParticles() const { return activeParticles_; }
        [[nodiscard]] bool isInitialized() const { return initialized_; }

    private:
        void createGPUResources();
        void destroyGPUResources();

        Config config_;
        ComputeShader simulateShader_;

        uint32_t particleSSBO_  = 0;  ///< Particle data (position, velocity, life, etc.)
        uint32_t counterBuffer_ = 0;  ///< Atomic counter for active particles
        uint32_t renderVAO_     = 0;  ///< VAO for rendering
        uint32_t renderProgram_ = 0;  ///< Render program (point sprites)

        int activeParticles_ = 0;
        float emitAccumulator_ = 0.0f;
        float time_ = 0.0f;
        bool initialized_ = false;
    };

} // namespace Nox
