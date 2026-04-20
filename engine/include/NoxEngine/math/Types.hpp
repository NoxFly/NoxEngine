// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

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

    struct Ray {
        Vec3 origin{ 0.0f };
        Vec3 direction{ 0.0f, 0.0f, -1.0f };

        [[nodiscard]] Vec3 at(float t) const {
            return origin + direction * t;
        }

        [[nodiscard]] bool intersectsAABB(const AABB& aabb, float& tMin, float& tMax) const {
            tMin = 0.0f;
            tMax = std::numeric_limits<float>::max();

            for (int i = 0; i < 3; ++i) {
                if (std::abs(direction[i]) < 1e-8f) {
                    if (origin[i] < aabb.min[i] || origin[i] > aabb.max[i])
                        return false;
                }
                else {
                    float invD = 1.0f / direction[i];
                    float t0 = (aabb.min[i] - origin[i]) * invD;
                    float t1 = (aabb.max[i] - origin[i]) * invD;
                    if (invD < 0.0f) std::swap(t0, t1);
                    tMin = std::max(tMin, t0);
                    tMax = std::min(tMax, t1);
                    if (tMax < tMin) return false;
                }
            }
            return true;
        }
    };

} // namespace Nox::Math
