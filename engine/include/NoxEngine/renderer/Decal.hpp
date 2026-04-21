// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/scene/SceneObject.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace Nox {

    /// A decal is a projected texture onto scene surfaces.
    /// Decals are rendered as oriented bounding boxes that project a texture
    /// onto the G-Buffer (deferred decals).
    class Decal : public SceneObject {
    public:
        explicit Decal(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
        ~Decal() override = default;

        /// Set the decal projection size.
        void setSize(float width, float height, float depth);

        /// Set the decal albedo texture path.
        void setAlbedoMap(const std::string& path);

        /// Set the decal normal map path.
        void setNormalMap(const std::string& path);

        /// Set the decal opacity.
        void setOpacity(float opacity) { opacity_ = opacity; }

        [[nodiscard]] float width() const { return width_; }
        [[nodiscard]] float height() const { return height_; }
        [[nodiscard]] float depth() const { return depth_; }
        [[nodiscard]] float opacity() const { return opacity_; }
        [[nodiscard]] const std::string& albedoMapPath() const { return albedoMapPath_; }
        [[nodiscard]] const std::string& normalMapPath() const { return normalMapPath_; }

        /// GPU data for the decal rendering.
        struct GpuData {
            uint32_t albedoTexture = 0;
            uint32_t normalTexture = 0;
        };

        GpuData& gpuData() { return gpuData_; }
        [[nodiscard]] const GpuData& gpuData() const { return gpuData_; }

    private:
        float width_  = 1.0f;
        float height_ = 1.0f;
        float depth_  = 1.0f;
        float opacity_ = 1.0f;
        std::string albedoMapPath_;
        std::string normalMapPath_;
        GpuData gpuData_{};
    };

    /// System for rendering deferred decals onto the G-Buffer.
    class DecalRenderer {
    public:
        DecalRenderer() = default;
        ~DecalRenderer();

        DecalRenderer(const DecalRenderer&) = delete;
        DecalRenderer& operator=(const DecalRenderer&) = delete;
        DecalRenderer(DecalRenderer&&) = default;
        DecalRenderer& operator=(DecalRenderer&&) = default;

        /// Initialize the decal rendering pipeline.
        void init();

        /// Render all decals onto the G-Buffer.
        /// Requires the G-Buffer depth texture for projection.
        void render(const std::vector<std::shared_ptr<Decal>>& decals,
                    const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix,
                    uint32_t gBufferDepthTex, uint32_t gBufferNormalTex,
                    int viewportWidth, int viewportHeight);

        [[nodiscard]] bool isInitialized() const { return initialized_; }

    private:
        void createUnitCube();

        uint32_t decalProgram_ = 0;
        uint32_t cubeVAO_      = 0;
        uint32_t cubeVBO_      = 0;
        uint32_t cubeIBO_      = 0;
        bool     initialized_  = false;
    };

} // namespace Nox
