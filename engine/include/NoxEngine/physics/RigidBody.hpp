// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <memory>
#include <variant>

namespace Nox {

    // ── Collider shape descriptors ─────────────────────────────────
    struct BoxShape {
        Math::Vec3 halfExtents{ 0.5f, 0.5f, 0.5f };
    };

    struct SphereShape {
        float radius = 0.5f;
    };

    struct CapsuleShape {
        float radius = 0.5f;
        float halfHeight = 0.5f;
    };

    using ColliderShape = std::variant<BoxShape, SphereShape, CapsuleShape>;

    // ── Motion type ────────────────────────────────────────────────
    enum class MotionType : uint8_t {
        Static,
        Dynamic,
        Kinematic
    };

    // ── Rigid body descriptor ──────────────────────────────────────
    struct RigidBodyDesc {
        ColliderShape shape = BoxShape{};
        MotionType    motionType = MotionType::Dynamic;
        Math::Vec3    position{ 0.0f };
        Math::Quat    rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
        float         mass = 1.0f;
        float         friction = 0.5f;
        float         restitution = 0.3f;
        float         linearDamping = 0.05f;
        float         angularDamping = 0.05f;
    };

    // ── Body handle ────────────────────────────────────────────────
    struct BodyHandle {
        uint32_t id = 0;
        [[nodiscard]] bool isValid() const { return id != 0; }
    };

} // namespace Nox
