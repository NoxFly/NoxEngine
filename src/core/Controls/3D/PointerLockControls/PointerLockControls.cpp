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
        m_sensitivity(1.0f),
        m_speed(5.0f)
    {
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
    }

    void PointerLockControls::unlockPointer() noexcept {
        m_renderer.setMouseFocus(false);
        m_renderer.setMouseGrab(false);
    }

    void PointerLockControls::update() {
        auto input = m_renderer.getInput();

        if(isLocked() && input->isKeyDown(SDL_SCANCODE_ESCAPE)) {
            unlockPointer();
            return;
        }

        if(!isLocked() && input->isMouseButtonDown(SDL_BUTTON_LEFT)) {
            lockPointer();
            return;
        }

        if(!m_renderer.isMouseFocused())
            return;

        auto deltaTime = m_renderer.getDeltaTime();

        if(deltaTime == 0.0)
            return;

        // orientation (look)
        if(input->isMouseMoving()) {
            auto mouseMov = input->getMouseMovement();

            float deltaX = glm::radians(mouseMov.x * m_sensitivity * deltaTime);
            float deltaY = glm::radians(mouseMov.y * m_sensitivity * deltaTime);

            // authorize the player to look up and down, left and right, but not to tilt his head
            // (usually done by keys [QA]/E)
            m_camera.orientate(V3D(deltaX, deltaY, 0.0f));
        }

        // displacement
        float speed = m_speed * deltaTime;

        if (input->isKeyDown(SDL_SCANCODE_W)) { // forward
            m_camera.move(m_camera.getForward() * speed);
        }
        if (input->isKeyDown(SDL_SCANCODE_S)) { // backward
            m_camera.move(-m_camera.getForward() * speed);
        }
        if (input->isKeyDown(SDL_SCANCODE_A)) { // left
            m_camera.move(-m_camera.getRight() * speed);
        }
        if (input->isKeyDown(SDL_SCANCODE_D)) { // right
            m_camera.move(m_camera.getRight() * speed);
        }
    }

}