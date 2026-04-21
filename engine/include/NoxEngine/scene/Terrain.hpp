// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Nox {

    /// A single terrain chunk with its own mesh and LOD level.
    /// The terrain is divided into a grid of chunks, each independently
    /// renderable at different LOD levels.
    struct TerrainChunk {
        int gridX     = 0;          ///< Chunk grid coordinate X
        int gridZ     = 0;          ///< Chunk grid coordinate Z
        int lodLevel  = 0;          ///< Current LOD level (0 = highest detail)
        bool loaded   = false;      ///< Whether chunk data is loaded
        bool gpuReady = false;      ///< Whether GPU buffers are uploaded

        uint32_t vao = 0;
        uint32_t vbo = 0;
        uint32_t ibo = 0;
        uint32_t indexCount = 0;

        Math::AABB aabb;            ///< World-space bounding box

        std::vector<float> heights; ///< Heightmap data for this chunk
    };

    /// Heightmap-based terrain with geomipmapping LOD.
    /// Supports terrain splatting (multi-texture blending by slope/altitude).
    class Terrain {
    public:
        /// Configuration for terrain generation.
        struct Config {
            int   chunkSize       = 64;       ///< Vertices per chunk edge (power of 2 + 1)
            int   numChunksX      = 16;       ///< Number of chunks along X
            int   numChunksZ      = 16;       ///< Number of chunks along Z
            float worldScale      = 1.0f;     ///< World units per vertex
            float heightScale     = 50.0f;    ///< Vertical scale factor
            int   maxLodLevels    = 5;        ///< Maximum LOD levels
            float lodDistanceFactor = 2.0f;   ///< LOD distance multiplier
        };

        Terrain() = default;
        ~Terrain();

        Terrain(const Terrain&) = delete;
        Terrain& operator=(const Terrain&) = delete;
        Terrain(Terrain&&) = default;
        Terrain& operator=(Terrain&&) = default;

        /// Initialize terrain from a heightmap image.
        void init(const Config& config = {});

        /// Load heightmap data from an image file.
        void loadHeightmap(const std::string& path);

        /// Generate flat terrain (for testing).
        void generateFlat(float height = 0.0f);

        /// Generate terrain from noise (procedural).
        void generateFromNoise(int seed = 42, float frequency = 0.01f, int octaves = 6);

        /// Update LOD levels and chunk streaming based on camera position.
        void update(const Math::Vec3& cameraPosition);

        /// Render visible chunks.
        void render(const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix,
                    const Math::Vec3& cameraPos);

        /// Set the splat textures for terrain painting.
        void setSplatTextures(uint32_t layer0, uint32_t layer1,
                              uint32_t layer2, uint32_t layer3);

        /// Set the splat map (R = layer0, G = layer1, B = layer2, A = layer3).
        void setSplatMap(uint32_t splatMapTex);

        [[nodiscard]] const Config& config() const { return config_; }
        [[nodiscard]] bool isInitialized() const { return initialized_; }

        /// Get terrain height at world XZ position.
        [[nodiscard]] float heightAt(float worldX, float worldZ) const;

        /// Get the normal at world XZ position.
        [[nodiscard]] Math::Vec3 normalAt(float worldX, float worldZ) const;

    private:
        void buildChunkMesh(TerrainChunk& chunk, int lod);
        void uploadChunk(TerrainChunk& chunk);
        void destroyChunk(TerrainChunk& chunk);
        void compilePipeline();
        int  computeLodLevel(const TerrainChunk& chunk, const Math::Vec3& camPos) const;

        Config config_;
        std::vector<TerrainChunk> chunks_;
        std::vector<float> globalHeightmap_;   ///< Full heightmap data

        uint32_t terrainProgram_ = 0;
        uint32_t splatTextures_[4] = {};
        uint32_t splatMapTex_ = 0;

        int heightmapWidth_  = 0;
        int heightmapHeight_ = 0;
        bool initialized_ = false;
    };

} // namespace Nox
