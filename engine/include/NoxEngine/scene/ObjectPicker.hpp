// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>
#include <NoxEngine/scene/Scene3D.hpp>
#include <NoxEngine/renderer/Camera.hpp>
#include <NoxEngine/platform/Input.hpp>

#include <memory>

namespace Nox {

    class Mesh;

    /// Picks objects in the scene by casting a ray from screen-space mouse coordinates.
    class ObjectPicker {
    public:
        ObjectPicker() = default;

        /// Pick the closest mesh at the given screen coordinates.
        /// Returns nullptr if nothing was hit.
        [[nodiscard]] std::shared_ptr<Mesh> pick(
            const Scene3D& scene,
            const PerspectiveCamera& camera,
            const Math::Vec2& screenPos,
            int viewportWidth, int viewportHeight) const;

        /// Convenience: pick using the current mouse position from Input.
        [[nodiscard]] std::shared_ptr<Mesh> pickFromMouse(
            const Scene3D& scene,
            const PerspectiveCamera& camera,
            const Input& input,
            int viewportWidth, int viewportHeight) const;

        /// Cast a ray from screen coordinates into world space.
        [[nodiscard]] static Math::Ray screenToRay(
            const PerspectiveCamera& camera,
            const Math::Vec2& screenPos,
            int viewportWidth, int viewportHeight);
    };

} // namespace Nox
