// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace Nox {

    class Mesh;

    /// Imposter system: automatically generates billboard textures of
    /// distant objects for efficient rendering.
    /// When objects are far away, their 3D mesh is replaced by a textured
    /// quad showing a pre-rendered view of the object.
    class ImposterSystem {
    public:
        struct ImposterData {
            uint32_t textureId = 0;        ///< Pre-rendered atlas texture
            Math::Vec3 position{ 0.0f };   ///< World position
            float size = 1.0f;             ///< Billboard size
            int atlasIndex = 0;            ///< Index in the atlas grid
        };

        struct Config {
            int   atlasSize       = 2048;  ///< Atlas texture resolution
            int   viewsPerRow     = 8;     ///< Number of view angles per row in atlas
            int   viewRows        = 4;     ///< Number of elevation angles
            float switchDistance   = 50.0f; ///< Distance at which to switch to imposter
            float fadeRange       = 10.0f; ///< Cross-fade distance range
        };

        ImposterSystem() = default;
        ~ImposterSystem();

        ImposterSystem(const ImposterSystem&) = delete;
        ImposterSystem& operator=(const ImposterSystem&) = delete;
        ImposterSystem(ImposterSystem&&) = default;
        ImposterSystem& operator=(ImposterSystem&&) = default;

        /// Initialize the imposter rendering system.
        void init(const Config& config = {});

        /// Generate an imposter atlas for a mesh by rendering it from multiple angles.
        /// Returns the atlas texture ID.
        [[nodiscard]] uint32_t generateAtlas(const std::shared_ptr<Mesh>& mesh);

        /// Render imposters visible from the camera.
        void render(const std::vector<ImposterData>& imposters,
                    const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix,
                    const Math::Vec3& cameraPos);

        [[nodiscard]] const Config& config() const { return config_; }
        [[nodiscard]] bool isInitialized() const { return initialized_; }

    private:
        Config config_;
        uint32_t captureFBO_  = 0;
        uint32_t captureRBO_  = 0;
        uint32_t billboardProgram_ = 0;
        uint32_t billboardVAO_ = 0;
        uint32_t billboardVBO_ = 0;
        bool     initialized_ = false;
    };

} // namespace Nox
