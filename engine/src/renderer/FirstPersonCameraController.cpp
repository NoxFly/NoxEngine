// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/CameraController.hpp>
#include <NoxEngine/renderer/Camera.hpp>
#include <NoxEngine/platform/Input.hpp>
#include <NoxEngine/platform/InputMapping.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <algorithm>
#include <cmath>

namespace Nox {

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

        // Movement on ground plane (layout-aware via InputMapping)
        Math::Vec3 velocity{ 0.0f };
        if (inputMapping_.isActionDown(input, Action::MoveForward))  { velocity += forwardXZ; }
        if (inputMapping_.isActionDown(input, Action::MoveBackward)) { velocity -= forwardXZ; }
        if (inputMapping_.isActionDown(input, Action::MoveRight))    { velocity += right; }
        if (inputMapping_.isActionDown(input, Action::MoveLeft))     { velocity -= right; }

        if (glm::length(velocity) > 0.0f) {
            velocity = glm::normalize(velocity);
        }

        Math::Vec3 pos = camera_.position() + velocity * moveSpeed_ * dt;
        pos.y = height_;
        camera_.setPosition(pos);

        Math::Vec3 lookDir{
            std::cos(pitchRad) * std::cos(yawRad),
            std::sin(pitchRad),
            std::cos(pitchRad) * std::sin(yawRad)
        };
        camera_.lookAt(pos + glm::normalize(lookDir));
    }

} // namespace Nox
