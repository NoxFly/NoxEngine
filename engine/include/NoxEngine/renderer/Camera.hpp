// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

// Individual camera headers
#include <NoxEngine/renderer/PerspectiveCamera.hpp>
#include <NoxEngine/renderer/OrthographicCamera.hpp>

#include <concepts>

namespace Nox {

    /// Concept satisfied by any camera type that provides view/projection matrices
    /// and a position. Used to unify render() overloads.
    template<typename T>
    concept CameraLike = requires(const T& cam) {
        { cam.viewMatrix() }       -> std::convertible_to<const Math::Mat4&>;
        { cam.projectionMatrix() } -> std::convertible_to<const Math::Mat4&>;
        { cam.position() }         -> std::convertible_to<const Math::Vec3&>;
    };

} // namespace Nox
