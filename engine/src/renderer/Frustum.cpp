// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/Frustum.hpp>

namespace Nox {

    void Frustum::extractFromMatrix(const Math::Mat4& vp) {
        // Left
        planes[0] = Math::Vec4(
            vp[0][3] + vp[0][0],
            vp[1][3] + vp[1][0],
            vp[2][3] + vp[2][0],
            vp[3][3] + vp[3][0]
        );
        // Right
        planes[1] = Math::Vec4(
            vp[0][3] - vp[0][0],
            vp[1][3] - vp[1][0],
            vp[2][3] - vp[2][0],
            vp[3][3] - vp[3][0]
        );
        // Bottom
        planes[2] = Math::Vec4(
            vp[0][3] + vp[0][1],
            vp[1][3] + vp[1][1],
            vp[2][3] + vp[2][1],
            vp[3][3] + vp[3][1]
        );
        // Top
        planes[3] = Math::Vec4(
            vp[0][3] - vp[0][1],
            vp[1][3] - vp[1][1],
            vp[2][3] - vp[2][1],
            vp[3][3] - vp[3][1]
        );
        // Near
        planes[4] = Math::Vec4(
            vp[0][3] + vp[0][2],
            vp[1][3] + vp[1][2],
            vp[2][3] + vp[2][2],
            vp[3][3] + vp[3][2]
        );
        // Far
        planes[5] = Math::Vec4(
            vp[0][3] - vp[0][2],
            vp[1][3] - vp[1][2],
            vp[2][3] - vp[2][2],
            vp[3][3] - vp[3][2]
        );

        // Normalize each plane
        for (auto& plane : planes) {
            float len = glm::length(Math::Vec3(plane));
            plane /= len;
        }
    }

    bool Frustum::containsAABB(const Math::AABB& aabb) const {
        for (const auto& plane : planes) {
            Math::Vec3 n(plane);
            // Find the positive vertex (farthest along normal)
            Math::Vec3 pVertex{
                (n.x >= 0.0f) ? aabb.max.x : aabb.min.x,
                (n.y >= 0.0f) ? aabb.max.y : aabb.min.y,
                (n.z >= 0.0f) ? aabb.max.z : aabb.min.z
            };

            if (glm::dot(n, pVertex) + plane.w < 0.0f) {
                return false;
            }
        }
        return true;
    }

} // namespace Nox
