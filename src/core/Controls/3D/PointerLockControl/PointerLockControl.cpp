/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "PointerLockControl.hpp"

#include "Console/Console.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace NoxEngine {

    PointerLockControl::PointerLockControl(Renderer& renderer, PerspectiveCamera& camera):
        m_renderer(renderer),
        m_camera(camera),
        m_sensitivity(0.1f),
        m_speed(5.0f)
    {
    }

    void PointerLockControl::update() {
        auto input = m_renderer.getInput();

        if(m_renderer.isMouseFocused() && input->isKeyDown(SDL_SCANCODE_ESCAPE)) {
            m_renderer.setMouseFocus(false);
            m_renderer.setMouseGrab(false);
            return;
        }

        if(!m_renderer.isMouseFocused() && input->isMouseButtonDown(SDL_BUTTON_LEFT)) {
            m_renderer.setMouseFocus(true);
            m_renderer.setMouseGrab(true);
            return;
        }

        if(!m_renderer.isMouseFocused())
            return;

        auto deltaTime = m_renderer.getDeltaTime();

        if(deltaTime == 0.0)
            return;

        if(input->isMouseMoving()) {
            auto mouseMov = input->getMouseMovement();

            mouseMov.x *= m_sensitivity;
            mouseMov.y *= m_sensitivity;

            glm::quat orientation = m_camera.getOrientation();
            glm::quat pitch = glm::angleAxis(glm::radians(mouseMov.y), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::quat yaw = glm::angleAxis(glm::radians(mouseMov.x), glm::vec3(0.0f, 1.0f, 0.0f));

            orientation = yaw * orientation * pitch;
            orientation = glm::normalize(orientation);
            m_camera.setOrientation(orientation);
        }


        // Keyboard movement handling
        glm::vec3 position = m_camera.getPosition();
        float speed = m_speed * deltaTime;

        if (input->isKeyDown(SDL_SCANCODE_Z)) {
            position += m_camera.getForward() * speed;
        }
        if (input->isKeyDown(SDL_SCANCODE_S)) {
            position -= m_camera.getForward() * speed;
        }
        if (input->isKeyDown(SDL_SCANCODE_Q)) {
            position -= m_camera.getRight() * speed;
        }
        if (input->isKeyDown(SDL_SCANCODE_D)) {
            position += m_camera.getRight() * speed;
        }

        m_camera.setPosition(position);
    }

}