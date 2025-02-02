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

    PointerLockControls::PointerLockControls(Renderer& renderer, PerspectiveCamera& camera):
        m_renderer(renderer),
        m_camera(camera),
        m_sensitivity(20.0f),
        m_speed(5.0f),
        m_velocity(0.0f),
        m_acceleration(10.0f),
        m_deceleration(5.0f),
        m_displacement(0.0f, 0.0f)
    {}

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
        const float dt = static_cast<float>(deltaTime);
        bool isMoving = false;

        if(m_renderer.isMouseFocused() && deltaTime > 0.0) {
            // orientation (look)
            const auto mouseMov = input->getMouseMovement();

            if(mouseMov.x != 0 || mouseMov.y != 0) {
                const auto mx = mouseMov.x * m_sensitivity * deltaTime;
                const auto my = mouseMov.y * m_sensitivity * deltaTime;

                const float deltaX = glm::radians(mx);
                const float deltaY = glm::radians(my);

                m_camera.orientate(V3D(-deltaY, -deltaX, 0.f));
            }

            // displacement
            if(input->isKeyDown(SDL_SCANCODE_W) || input->isKeyDown(SDL_SCANCODE_S) ||
            input->isKeyDown(SDL_SCANCODE_A) || input->isKeyDown(SDL_SCANCODE_D)) {
                isMoving = true;
            }
        }

        // acceleration
        if(isMoving) {
            if(m_velocity < m_speed) {
                m_velocity = std::min(m_velocity + m_acceleration * dt, m_speed);
            }

            if(input->isKeyDown(SDL_SCANCODE_W))
                m_displacement.y += 1.0f;
            if(input->isKeyDown(SDL_SCANCODE_S))
                m_displacement.y -= 1.0f;
            if(input->isKeyDown(SDL_SCANCODE_A))
                m_displacement.x -= 1.0f;
            if(input->isKeyDown(SDL_SCANCODE_D))
                m_displacement.x += 1.0f;

            m_displacement.x = std::clamp(m_displacement.x, -1.0f, 1.0f);
            m_displacement.y = std::clamp(m_displacement.y, -1.0f, 1.0f);
        }
        // deceleration
        else {
            if(m_velocity > 0.0f) {
                m_velocity = std::max(0.0f, m_velocity - m_deceleration * dt);
            }
            else {
                m_displacement.x = 0;
                m_displacement.y = 0;
            }
        }

        const float speed = m_velocity * dt;

        if(speed > 0.0f) {
            const auto forward = m_camera.getForward();
            const auto right = m_camera.getRight();

            const auto displacement = right * m_displacement.x + forward * m_displacement.y;
            m_camera.move(displacement * speed);
        }
    }

}