// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <array>

namespace Nox {

    struct Frustum {
        // Plane: ax + by + cz + d = 0, stored as Vec4(a,b,c,d)
        // Planes: Left, Right, Bottom, Top, Near, Far
        std::array<Math::Vec4, 6> planes;

        void extractFromMatrix(const Math::Mat4& viewProj);

        [[nodiscard]] bool containsAABB(const Math::AABB& aabb) const;
    };

} // namespace Nox
