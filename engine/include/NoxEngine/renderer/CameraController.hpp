// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

namespace Nox {

    class PerspectiveCamera;
    class Input;

    // ── OrbitCameraController ──────────────────────────────────────
    // Orbits around a target point. Mouse drag to rotate, scroll to zoom.
    class OrbitCameraController {
    public:
        explicit OrbitCameraController(PerspectiveCamera& camera);

        void update(const Input& input, float dt);

        void setTarget(const Math::Vec3& target);
        void setDistance(float distance);
        void setRotationSpeed(float speed) { rotationSpeed_ = speed; }
        void setZoomSpeed(float speed) { zoomSpeed_ = speed; }
        void setMinDistance(float d) { minDistance_ = d; }
        void setMaxDistance(float d) { maxDistance_ = d; }

    private:
        PerspectiveCamera& camera_;
        Math::Vec3 target_{ 0.0f };
        float yaw_   = 0.0f;   // degrees
        float pitch_ = 30.0f;  // degrees
        float distance_ = 5.0f;
        float rotationSpeed_ = 0.3f;
        float zoomSpeed_ = 1.0f;
        float minDistance_ = 0.5f;
        float maxDistance_ = 100.0f;
    };

    // ── FlyCameraController ────────────────────────────────────────
    // Free-flight camera. WASD to move, mouse drag to look around.
    class FlyCameraController {
    public:
        explicit FlyCameraController(PerspectiveCamera& camera);

        void update(const Input& input, float dt);

        void setMoveSpeed(float speed) { moveSpeed_ = speed; }
        void setLookSpeed(float speed) { lookSpeed_ = speed; }

    private:
        PerspectiveCamera& camera_;
        float yaw_   = -90.0f; // degrees, facing -Z by default
        float pitch_ = 0.0f;   // degrees
        float moveSpeed_ = 5.0f;
        float lookSpeed_ = 0.15f;
    };

    // ── FirstPersonCameraController ────────────────────────────────
    // First-person style. WASD to move (constrained to ground plane), mouse to look.
    class FirstPersonCameraController {
    public:
        explicit FirstPersonCameraController(PerspectiveCamera& camera);

        void update(const Input& input, float dt);

        void setMoveSpeed(float speed) { moveSpeed_ = speed; }
        void setLookSpeed(float speed) { lookSpeed_ = speed; }
        void setHeight(float h) { height_ = h; }

    private:
        PerspectiveCamera& camera_;
        float yaw_   = -90.0f;
        float pitch_ = 0.0f;
        float moveSpeed_ = 5.0f;
        float lookSpeed_ = 0.15f;
        float height_ = 1.7f;
    };

} // namespace Nox
