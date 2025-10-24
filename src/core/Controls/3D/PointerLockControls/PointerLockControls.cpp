/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "PointerLockControls.hpp"

#include "Console/Console.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace NoxEngine {

    PointerLockControls::PointerLockControls(Renderer& renderer, PerspectiveCamera& camera, const bool enableInterpolation):
        m_renderer(renderer),
        m_camera(camera),
        m_sensitivity(20.0f),
        m_speed(5.0f),
        m_velocity(0.0f),
        m_acceleration(10.0f),
        m_deceleration(5.0f),
        m_lerpFactor(10.0f),
        m_displacement(0.0f, 0.0f, 0.0f),
        m_smoothRotation(0.0f, 0.0f),
        m_ignoreNextMouseMove(false),
        m_enableInterpolation(enableInterpolation),
        m_floating(false)
    {}

    void PointerLockControls::enableInterpolation() noexcept {
        m_enableInterpolation = true;
    }

    void PointerLockControls::disableInterpolation() noexcept {
        m_enableInterpolation = false;
    }

    bool PointerLockControls::isInterpolationEnabled() const noexcept {
        return m_enableInterpolation;
    }

    void PointerLockControls::setFloatingState(const bool floating) noexcept {
        m_floating = floating;
    }

    bool PointerLockControls::isFloating() const noexcept {
        return m_floating;
    }

    void PointerLockControls::setLerpFactor(const float factor) noexcept {
        m_lerpFactor = factor;
    }

    void PointerLockControls::setSpeed(const float speed) noexcept {
        m_speed = speed;
    }

    void PointerLockControls::setSensitivity(const float sensitivity) noexcept {
        m_sensitivity = sensitivity;
    }

    float PointerLockControls::getSpeed() const noexcept {
        return m_speed;
    }

    float PointerLockControls::getSensitivity() const noexcept {
        return m_sensitivity;
    }

    bool PointerLockControls::isLocked() noexcept {
        return m_renderer.isMouseFocused();
    }

    void PointerLockControls::lockPointer() noexcept {
        m_renderer.setMouseFocus(true);
        m_renderer.setMouseGrab(true);
        m_ignoreNextMouseMove = true; // Ignore the first mouse movement after locking
    }

    void PointerLockControls::unlockPointer() noexcept {
        m_renderer.setMouseFocus(false);
        m_renderer.setMouseGrab(false);
    }

    void PointerLockControls::update() {
        auto input = m_renderer.getInput();

        if(isLocked() && input->isKeyDown(SDL_SCANCODE_ESCAPE)) {
            unlockPointer();
        }
        else if(!isLocked() && input->isMouseButtonDown(SDL_BUTTON_LEFT)) {
            lockPointer();
        }

        auto deltaTime = m_renderer.getDeltaTime();
        bool isMoving = false;

        if(m_renderer.isMouseFocused() && deltaTime > 0.0f) {
            // orientation (look)
            const auto mouseMov = input->getMouseMovement();

            if(mouseMov.x != 0 || mouseMov.y != 0) {
                // Ignore first mouse movement after locking to prevent camera jump
                if(m_ignoreNextMouseMove) {
                    m_ignoreNextMouseMove = false;
                }
                else {
                    const auto mx = mouseMov.x * m_sensitivity * deltaTime;
                    const auto my = mouseMov.y * m_sensitivity * deltaTime;

                    if(m_enableInterpolation) {
                        // Accumulate raw rotation input (inverted for natural camera movement)
                        m_smoothRotation.x -= my;
                        m_smoothRotation.y -= mx;
                    }
                    else {
                        // Apply rotation directly without interpolation
                        const float deltaX = glm::radians(-mx);
                        const float deltaY = glm::radians(-my);
                        m_camera.orientate(V3D(deltaY, deltaX, 0.f));
                    }
                }
            }

            if(m_enableInterpolation) {
                // Apply smoothed rotation with interpolation
                const float rotationLerpFactor = 15.0f * deltaTime; // Adjust for camera rotation smoothness
                
                if(glm::length(m_smoothRotation) > 0.001f) {
                    const float deltaX = glm::radians(m_smoothRotation.y * rotationLerpFactor);
                    const float deltaY = glm::radians(m_smoothRotation.x * rotationLerpFactor);

                    m_camera.orientate(V3D(deltaY, deltaX, 0.f));

                    // Decay the smooth rotation
                    m_smoothRotation *= (1.0f - rotationLerpFactor);
                }
            }

            // displacement
            if(
                input->isKeyDown(SDL_SCANCODE_W) || input->isKeyDown(SDL_SCANCODE_S)
                || input->isKeyDown(SDL_SCANCODE_A) || input->isKeyDown(SDL_SCANCODE_D)
                || (m_floating && (input->isKeyDown(SDL_SCANCODE_SPACE) || input->isKeyDown(SDL_SCANCODE_LCTRL)))
            ) {
                isMoving = true;
            }
        }

        // Target displacement based on input
        V3D targetDisplacement(0.0f, 0.0f, 0.0f);
        
        if(isMoving) {
            if(input->isKeyDown(SDL_SCANCODE_W))
                targetDisplacement.y += 1.0f;
            
            if(input->isKeyDown(SDL_SCANCODE_S))
                targetDisplacement.y -= 1.0f;
            
            if(input->isKeyDown(SDL_SCANCODE_A))
                targetDisplacement.x -= 1.0f;
            
            if(input->isKeyDown(SDL_SCANCODE_D))
                targetDisplacement.x += 1.0f;

            if(m_floating) {
                if(input->isKeyDown(SDL_SCANCODE_SPACE))
                    targetDisplacement.z += 1.0f;

                if(input->isKeyDown(SDL_SCANCODE_LCTRL))
                    targetDisplacement.z -= 1.0f;
            }

            // Normalize diagonal movement
            if(glm::length(targetDisplacement) > 1.0f)
                targetDisplacement = glm::normalize(targetDisplacement);
        }

        if(m_enableInterpolation) {
            // Smooth interpolation of displacement (lerp)
            const float lerpFactor = m_lerpFactor * deltaTime;
            
            m_displacement.x = glm::mix(m_displacement.x, targetDisplacement.x, lerpFactor);
            m_displacement.y = glm::mix(m_displacement.y, targetDisplacement.y, lerpFactor);
            m_displacement.z = glm::mix(m_displacement.z, targetDisplacement.z, lerpFactor);

            // acceleration
            if(isMoving) {
                if(m_velocity < m_speed) {
                    m_velocity = std::min(m_velocity + m_acceleration * deltaTime, m_speed);
                }
            }
            // deceleration
            else {
                if(m_velocity > 0.0f) {
                    m_velocity = std::max(0.0f, m_velocity - m_deceleration * deltaTime);
                }
            }
        }
        else {
            // Direct displacement without interpolation
            m_displacement = targetDisplacement;
            m_velocity = isMoving
                ? m_speed
                : 0.0f;
        }

        const float speed = m_velocity * deltaTime;

        if(glm::length(m_displacement) > 0.001f) {
            const auto forward = m_camera.getForward();
            const auto right = m_camera.getRight();
            const auto up = m_camera.getUp();

            const auto displacement = right * m_displacement.x
                + forward * m_displacement.y
                + up * m_displacement.z;
            
            // Move the camera directly without applying rotation again
            // (forward and right vectors already account for camera orientation)
            const auto newPosition = m_camera.getPosition() + displacement * speed;
            m_camera.moveTo(newPosition);
        }
    }

}