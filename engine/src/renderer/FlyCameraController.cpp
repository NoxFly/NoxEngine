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

} // namespace Nox
