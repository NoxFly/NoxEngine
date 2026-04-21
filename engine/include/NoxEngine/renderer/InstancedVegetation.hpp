// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace Nox {

    class Mesh;

    /// GPU-instanced vegetation rendering.
    /// Places instances of a base mesh (grass, trees) at positions generated
    /// from a placement map or procedural distribution.
    class InstancedVegetation {
    public:
        /// Configuration for vegetation placement.
        struct Config {
            float density       = 1.0f;       ///< Instances per world unit²
            float minScale      = 0.8f;       ///< Minimum random scale
            float maxScale      = 1.2f;       ///< Maximum random scale
            float maxSlopeAngle = 30.0f;      ///< Max terrain slope for placement (degrees)
            float drawDistance   = 100.0f;     ///< Max rendering distance from camera
            float fadeDistance   = 20.0f;      ///< Distance over which instances fade out
            int   maxInstances   = 100000;     ///< Max instance count
        };

        InstancedVegetation() = default;
        ~InstancedVegetation();

        InstancedVegetation(const InstancedVegetation&) = delete;
        InstancedVegetation& operator=(const InstancedVegetation&) = delete;
        InstancedVegetation(InstancedVegetation&&) = default;
        InstancedVegetation& operator=(InstancedVegetation&&) = default;

        /// Initialize the instancing system with a mesh prototype.
        void init(const std::shared_ptr<Mesh>& prototype, const Config& config = {});

        /// Generate instance positions from terrain data.
        /// Distributes instances based on slope and height constraints.
        void generateFromTerrain(const std::vector<float>& heightmap,
                                  int hmWidth, int hmHeight,
                                  float terrainScale, float heightScale);

        /// Manually set instance transforms.
        void setInstanceTransforms(const std::vector<Math::Mat4>& transforms);

        /// Update visible instances based on camera distance (frustum + distance culling).
        void update(const Math::Vec3& cameraPosition, const Math::Mat4& viewProjMatrix);

        /// Render all visible instances in a single draw call.
        void render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix);

        [[nodiscard]] int totalInstances() const { return totalInstances_; }
        [[nodiscard]] int visibleInstances() const { return visibleInstances_; }
        [[nodiscard]] bool isInitialized() const { return initialized_; }

    private:
        void uploadInstanceData();

        Config config_;
        std::shared_ptr<Mesh> prototype_;

        std::vector<Math::Mat4> allTransforms_;    ///< All instance transforms
        std::vector<Math::Mat4> visibleTransforms_; ///< Currently visible subset

        uint32_t instanceVBO_ = 0;    ///< Instance data buffer
        uint32_t instanceVAO_ = 0;    ///< VAO with instanced attributes
        uint32_t instanceProgram_ = 0;

        int totalInstances_   = 0;
        int visibleInstances_ = 0;
        bool initialized_ = false;
    };

} // namespace Nox
