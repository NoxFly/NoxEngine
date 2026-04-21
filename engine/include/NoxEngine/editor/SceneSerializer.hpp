// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/scene/Scene3D.hpp>

#include <filesystem>

namespace Nox {

    /// Serializes and deserializes a Scene3D to/from a JSON file.
    /// Supports meshes with geometry factories, materials, and lights.
    class SceneSerializer {
    public:
        /// Save the scene to a JSON file.
        /// Returns true on success.
        [[nodiscard]] static bool save(const Scene3D& scene, const std::filesystem::path& path);

        /// Load a scene from a JSON file, replacing current scene contents.
        /// Returns true on success.
        [[nodiscard]] static bool load(Scene3D& scene, const std::filesystem::path& path);
    };

} // namespace Nox
