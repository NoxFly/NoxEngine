// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/scene/Light.hpp>

#include <cstdint>
#include <vector>

namespace Nox {

    /// Clustered forward/deferred lighting for handling hundreds of lights efficiently.
    /// Divides the view frustum into 3D clusters and assigns lights to clusters.
    /// During the lighting pass, each pixel only evaluates lights in its cluster.
    class ClusteredLighting {
    public:
        /// Configuration for the cluster grid.
        struct Config {
            int gridSizeX   = 16;   ///< Number of clusters along X axis
            int gridSizeY   = 9;    ///< Number of clusters along Y axis
            int gridSizeZ   = 24;   ///< Number of clusters along Z (depth) axis
            int maxLightsPerCluster = 128;  ///< Max lights per cluster
        };

        ClusteredLighting() = default;
        ~ClusteredLighting();

        ClusteredLighting(const ClusteredLighting&) = delete;
        ClusteredLighting& operator=(const ClusteredLighting&) = delete;
        ClusteredLighting(ClusteredLighting&&) = default;
        ClusteredLighting& operator=(ClusteredLighting&&) = default;

        /// Initialize GPU resources (SSBOs, compute shader).
        void init(const Config& config = {});

        /// Update cluster assignments for the current frame.
        /// @param viewMatrix     Camera view matrix
        /// @param projMatrix     Camera projection matrix
        /// @param near           Camera near plane
        /// @param far            Camera far plane
        /// @param pointLights    Active point lights
        /// @param viewportWidth  Width of the viewport
        /// @param viewportHeight Height of the viewport
        void update(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix,
                    float near, float far,
                    const std::vector<std::shared_ptr<PointLight>>& pointLights,
                    int viewportWidth, int viewportHeight);

        /// Bind the light and cluster SSBOs for use in the lighting shader.
        void bind() const;

        [[nodiscard]] const Config& config() const { return config_; }
        [[nodiscard]] bool isInitialized() const { return initialized_; }
        [[nodiscard]] int totalClusters() const {
            return config_.gridSizeX * config_.gridSizeY * config_.gridSizeZ;
        }

    private:
        Config   config_;
        uint32_t lightSSBO_       = 0;   ///< SSBO containing light data
        uint32_t clusterSSBO_     = 0;   ///< SSBO containing cluster-light assignments
        uint32_t clusterAABBSSBO_ = 0;   ///< SSBO containing cluster AABBs
        uint32_t lightIndexSSBO_  = 0;   ///< SSBO containing light index lists
        uint32_t assignProgram_   = 0;   ///< Compute shader for light-cluster assignment
        bool     initialized_    = false;
    };

} // namespace Nox
