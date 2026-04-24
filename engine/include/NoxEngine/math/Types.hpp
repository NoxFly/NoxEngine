// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
                    if (origin[i] < aabb.min[i] || origin[i] > aabb.max[i]) {
                        return false;
                    }
                }
                else {
                    float invD = 1.0f / direction[i];
                    float t0 = (aabb.min[i] - origin[i]) * invD;
                    float t1 = (aabb.max[i] - origin[i]) * invD;

                    if (invD < 0.0f) {
                        std::swap(t0, t1);
                    }

                    tMin = std::max(tMin, t0);
                    tMax = std::min(tMax, t1);

                    if (tMax < tMin) {
                        return false;
                    }
                }
            }

            return true;
        }
    };

    // ── Free-function math wrappers ────────────────────────────────
    // These forward to GLM so consumers never need to include <glm/...> directly.

    /// @brief Linear interpolation between two values.
    template<typename T, typename U>
    [[nodiscard]] inline T mix(const T& a, const T& b, U t) { return glm::mix(a, b, t); }

    /// @brief Normalize a vector.
    template<typename T>
    [[nodiscard]] inline T normalize(const T& v) { return glm::normalize(v); }

    /// @brief Cross product of two vectors.
    [[nodiscard]] inline Vec3 cross(const Vec3& a, const Vec3& b) { return glm::cross(a, b); }

    /// @brief Dot product of two vectors.
    template<typename T>
    [[nodiscard]] inline float dot(const T& a, const T& b) { return glm::dot(a, b); }

    /// @brief Length of a vector.
    template<typename T>
    [[nodiscard]] inline float length(const T& v) { return glm::length(v); }

    /// @brief Convert degrees to radians.
    [[nodiscard]] inline float radians(float deg) { return glm::radians(deg); }

    /// @brief Convert radians to degrees.
    [[nodiscard]] inline float degrees(float rad) { return glm::degrees(rad); }

    /// @brief Build a translation matrix.
    [[nodiscard]] inline Mat4 translate(const Mat4& m, const Vec3& v) { return glm::translate(m, v); }

    /// @brief Build a rotation matrix.
    [[nodiscard]] inline Mat4 rotate(const Mat4& m, float angle, const Vec3& axis) { return glm::rotate(m, angle, axis); }

    /// @brief Build a scale matrix.
    [[nodiscard]] inline Mat4 scale(const Mat4& m, const Vec3& v) { return glm::scale(m, v); }

} // namespace Nox::Math
