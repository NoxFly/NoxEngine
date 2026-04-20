// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/CameraController.hpp>
#include <NoxEngine/renderer/Camera.hpp>
#include <NoxEngine/platform/Input.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <algorithm>
#include <cmath>

namespace Nox {

    // ── OrbitCameraController ──────────────────────────────────────

    OrbitCameraController::OrbitCameraController(PerspectiveCamera& camera)
        : camera_(camera)
        , target_(camera.target())
        , distance_(glm::length(camera.position() - camera.target())) {
        // Compute initial yaw/pitch from camera position relative to target
        Math::Vec3 dir = glm::normalize(camera.position() - target_);
        pitch_ = glm::degrees(std::asin(dir.y));
        yaw_   = glm::degrees(std::atan2(dir.z, dir.x));
    }

    void OrbitCameraController::update(const Input& input, float /*dt*/) {
        // Rotate with right mouse button
        if (input.isMouseButtonDown(MouseButton::Right)) {
            auto delta = input.mouseDelta();
            yaw_   += delta.x * rotationSpeed_;
            pitch_ -= delta.y * rotationSpeed_;
            pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
        }

        // Zoom with scroll
        float scroll = input.scrollDelta();
        if (scroll != 0.0f) {
            distance_ -= scroll * zoomSpeed_;
            distance_ = std::clamp(distance_, minDistance_, maxDistance_);
        }

        // Pan with middle mouse button
        if (input.isMouseButtonDown(MouseButton::Middle)) {
            auto delta = input.mouseDelta();
            float yawRad   = glm::radians(yaw_);
            float pitchRad = glm::radians(pitch_);

            Math::Vec3 forward{
                std::cos(pitchRad) * std::cos(yawRad),
                std::sin(pitchRad),
                std::cos(pitchRad) * std::sin(yawRad)
            };
            Math::Vec3 right = glm::normalize(glm::cross(forward, Math::Vec3(0.0f, 1.0f, 0.0f)));
            Math::Vec3 up    = glm::normalize(glm::cross(right, forward));

            float panScale = distance_ * 0.002f;
            target_ -= right * delta.x * panScale;
            target_ += up    * delta.y * panScale;
        }

        // Update camera position from spherical coordinates
        float yawRad   = glm::radians(yaw_);
        float pitchRad = glm::radians(pitch_);
        Math::Vec3 offset{
            std::cos(pitchRad) * std::cos(yawRad),
            std::sin(pitchRad),
            std::cos(pitchRad) * std::sin(yawRad)
        };

        camera_.setPosition(target_ + offset * distance_);
        camera_.lookAt(target_);
    }

    void OrbitCameraController::setTarget(const Math::Vec3& target) {
        target_ = target;
    }

    void OrbitCameraController::setDistance(float distance) {
        distance_ = std::clamp(distance, minDistance_, maxDistance_);
    }

    // ── FlyCameraController ────────────────────────────────────────

    FlyCameraController::FlyCameraController(PerspectiveCamera& camera)
        : camera_(camera) {
        Math::Vec3 dir = glm::normalize(camera.target() - camera.position());
        pitch_ = glm::degrees(std::asin(dir.y));
        yaw_   = glm::degrees(std::atan2(dir.z, dir.x));
    }

    void FlyCameraController::update(const Input& input, float dt) {
        // Look around with right mouse button
        if (input.isMouseButtonDown(MouseButton::Right)) {
            auto delta = input.mouseDelta();
            yaw_   += delta.x * lookSpeed_;
            pitch_ -= delta.y * lookSpeed_;
            pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
        }

        float yawRad   = glm::radians(yaw_);
        float pitchRad = glm::radians(pitch_);

        Math::Vec3 forward{
            std::cos(pitchRad) * std::cos(yawRad),
            std::sin(pitchRad),
            std::cos(pitchRad) * std::sin(yawRad)
        };
        forward = glm::normalize(forward);
        Math::Vec3 right = glm::normalize(glm::cross(forward, Math::Vec3(0.0f, 1.0f, 0.0f)));
        Math::Vec3 up    = glm::normalize(glm::cross(right, forward));

        // WASD movement
        Math::Vec3 velocity{ 0.0f };
        if (input.isKeyDown(Key::W)) { velocity += forward; }
        if (input.isKeyDown(Key::S)) { velocity -= forward; }
        if (input.isKeyDown(Key::D)) { velocity += right; }
        if (input.isKeyDown(Key::A)) { velocity -= right; }
        if (input.isKeyDown(Key::Space)) { velocity += up; }
        if (input.isKeyDown(Key::LShift)) { velocity -= up; }

        if (glm::length(velocity) > 0.0f) {
            velocity = glm::normalize(velocity);
        }

        Math::Vec3 pos = camera_.position() + velocity * moveSpeed_ * dt;
        camera_.setPosition(pos);
        camera_.lookAt(pos + forward);
    }

    // ── FirstPersonCameraController ────────────────────────────────

    FirstPersonCameraController::FirstPersonCameraController(PerspectiveCamera& camera)
        : camera_(camera)
        , height_(camera.position().y) {
        Math::Vec3 dir = glm::normalize(camera.target() - camera.position());
        pitch_ = glm::degrees(std::asin(dir.y));
        yaw_   = glm::degrees(std::atan2(dir.z, dir.x));
    }

    void FirstPersonCameraController::update(const Input& input, float dt) {
        // Look around with right mouse button
        if (input.isMouseButtonDown(MouseButton::Right)) {
            auto delta = input.mouseDelta();
            yaw_   += delta.x * lookSpeed_;
            pitch_ -= delta.y * lookSpeed_;
            pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
        }

        float yawRad   = glm::radians(yaw_);
        float pitchRad = glm::radians(pitch_);

        // Forward direction projected onto ground plane (XZ)
        Math::Vec3 forwardXZ{
            std::cos(yawRad),
            0.0f,
            std::sin(yawRad)
        };
        forwardXZ = glm::normalize(forwardXZ);
        Math::Vec3 right = glm::normalize(glm::cross(forwardXZ, Math::Vec3(0.0f, 1.0f, 0.0f)));

        // Movement on ground plane
        Math::Vec3 velocity{ 0.0f };
        if (input.isKeyDown(Key::W)) { velocity += forwardXZ; }
        if (input.isKeyDown(Key::S)) { velocity -= forwardXZ; }
        if (input.isKeyDown(Key::D)) { velocity += right; }
        if (input.isKeyDown(Key::A)) { velocity -= right; }

        if (glm::length(velocity) > 0.0f) {
            velocity = glm::normalize(velocity);
        }

        Math::Vec3 pos = camera_.position() + velocity * moveSpeed_ * dt;
        pos.y = height_;

        // Look direction uses full pitch
        Math::Vec3 lookDir{
            std::cos(pitchRad) * std::cos(yawRad),
            std::sin(pitchRad),
            std::cos(pitchRad) * std::sin(yawRad)
        };

        camera_.setPosition(pos);
        camera_.lookAt(pos + lookDir);
    }

} // namespace Nox
