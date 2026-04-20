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

    OrbitCameraController::OrbitCameraController(PerspectiveCamera& camera)
        : camera_(camera)
        , target_(camera.target())
        , distance_(glm::length(camera.position() - camera.target())) {
        // Only compute pitch/yaw if distance is non-zero
        if (distance_ > 0.001f) {
            Math::Vec3 dir = glm::normalize(camera.position() - target_);
            pitch_ = glm::degrees(std::asin(dir.y));
            yaw_   = glm::degrees(std::atan2(dir.z, dir.x));
        }
    }

    void OrbitCameraController::syncFromCamera() {
        target_ = camera_.target();
        distance_ = glm::length(camera_.position() - target_);
        if (distance_ > 0.001f) {
            Math::Vec3 dir = glm::normalize(camera_.position() - target_);
            pitch_ = glm::degrees(std::asin(dir.y));
            yaw_   = glm::degrees(std::atan2(dir.z, dir.x));
        }
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

} // namespace Nox
