// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Nox::Math {

    using Vec2 = glm::vec2;
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec4;
    using Mat3 = glm::mat3;
    using Mat4 = glm::mat4;
    using Quat = glm::quat;

    struct AABB {
        Vec3 min{ 0.0f };
        Vec3 max{ 0.0f };

        [[nodiscard]] bool contains(const Vec3& point) const {
            return point.x >= min.x && point.x <= max.x
                && point.y >= min.y && point.y <= max.y
                && point.z >= min.z && point.z <= max.z;
        }

        [[nodiscard]] bool intersects(const AABB& other) const {
            return min.x <= other.max.x && max.x >= other.min.x
                && min.y <= other.max.y && max.y >= other.min.y
                && min.z <= other.max.z && max.z >= other.min.z;
        }
    };

} // namespace Nox::Math
