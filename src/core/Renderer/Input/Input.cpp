/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Input.hpp"

#include <iostream>
#include <glm/gtx/transform.hpp>

#include "Console/Console.hpp"


namespace NoxEngine {

    Input::Input():
        m_mouseX(0), m_mouseY(0), m_mouseRelX(0), m_mouseRelY(0),
        m_oldMouseX(0), m_oldMouseY(0), m_mouseMotion(0, 0),
        m_shouldClose(false)
    {
        for(int i=0; i < SDL_NUM_SCANCODES; i++)
            m_keys[i] = KeyState::UP;

        for(int i=0; i < 8; i++)
            m_mouseButtons[i] = KeyState::UP;

        m_wheel = WheelState::NONE;
    }

    Input::~Input() {

    }


    void Input::updateEvents() noexcept {
        finalizeLastFrameState();
        update();
    }

    void Input::update() noexcept {
        SDL_Event events;

        while(SDL_PollEvent(&events)) {
            SDL_Scancode kc = events.key.keysym.scancode;
            Uint8 mb = events.button.button;

            switch(events.type) {
                case SDL_WINDOWEVENT:
                    if(events.window.event == SDL_WINDOWEVENT_CLOSE)
                        m_shouldClose = true;
                    break;

                // ----------------------------- KEYBOARD
                // key down
                case SDL_KEYDOWN:
                    if(m_keys[kc] == KeyState::UP)
                        m_keys[kc] = KeyState::PRESSED;
                    break;

                // key up
                case SDL_KEYUP:
                    if(m_keys[kc] == KeyState::DOWN)
                        m_keys[kc] = KeyState::RELEASED;
                    break;

                // ----------------------------- MOUSE
                // mouse button pressed
                case SDL_MOUSEBUTTONDOWN:
                    if(m_mouseButtons[mb] == KeyState::UP)
                        m_mouseButtons[mb] = KeyState::PRESSED;
                    break;

                // mouse button released
                case SDL_MOUSEBUTTONUP:
                    if(m_mouseButtons[mb] == KeyState::DOWN)
                        m_mouseButtons[mb] = KeyState::RELEASED;
                    break;

                // mouse move
                case SDL_MOUSEMOTION:
                    m_oldMouseX = m_mouseX;
                    m_oldMouseY = m_mouseY;

                    m_mouseMotion = V2D(events.motion.xrel, events.motion.yrel);
                    
                    m_mouseX = events.motion.x;
                    m_mouseY = events.motion.y;
                    break;

                case SDL_MOUSEWHEEL:
                    if(events.wheel.y > 0) {
                        m_wheel = WheelState::UP;
                    }
                    else if(events.wheel.y < 0) {
                        m_wheel = WheelState::DOWN;
                    }
                    break;
            }
        }
    }

    void Input::finalizeLastFrameState() noexcept {
        m_wheel = WheelState::NONE;

        for(auto& key : m_keys) {
            if(key == KeyState::PRESSED)
                key = KeyState::DOWN;
            else if(key == KeyState::RELEASED)
                key = KeyState::UP;
        }

        for(auto& button : m_mouseButtons) {
            if(button == KeyState::PRESSED)
                button = KeyState::DOWN;
            else if(button == KeyState::RELEASED)
                button = KeyState::UP;
        }
    }



    /* KEYBOARD */


    bool Input::isKeyDown(const SDL_Scancode key) const noexcept {
        return m_keys[key] == KeyState::DOWN;
    }

    bool Input::isKeyUp(const SDL_Scancode key) const noexcept {
        return m_keys[key] == KeyState::UP;
    }

    bool Input::isKeyPressed(const SDL_Scancode key) const noexcept {
        return m_keys[key] == KeyState::PRESSED;
    }

    bool Input::isKeyReleased(const SDL_Scancode key) const noexcept {
        return m_keys[key] == KeyState::RELEASED;
    }


    /* MOUSE & WHEEL */

    bool Input::isMouseButtonDown(const Uint8 button) const noexcept {
        return m_mouseButtons[button] == KeyState::DOWN;
    }

    bool Input::isMouseButtonUp(const Uint8 button) const noexcept {
        return m_mouseButtons[button] == KeyState::UP;
    }

    bool Input::isMouseButtonPressed(const Uint8 button) const noexcept {
        return m_mouseButtons[button] == KeyState::PRESSED;
    }

    bool Input::isMouseButtonReleased(const Uint8 button) const noexcept {
        return m_mouseButtons[button] == KeyState::RELEASED;
    }

    bool Input::isMouseMoving() const noexcept {
        V2D v = m_mouseMotion;
        return v.x != 0 && v.y != 0;
    }

    int Input::getMouseX() const noexcept {
        return m_mouseX;
    }

    // actual Y
    int Input::getMouseY() const noexcept {
        return m_mouseY;
    }

    WheelState Input::wheelScroll() const noexcept {
        return m_wheel;
    }

    V2D Input::getMouseMovement() const noexcept {
        int x, y;
        SDL_GetRelativeMouseState(&x, &y);
        return V2D(x, y);
    }

    V2D Input::getMousePosition() const noexcept {
        return V2D(m_mouseX, m_mouseY);
    }

    V2D Input::getMousePointFromCenter(SDL_Window* window) const noexcept {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        V2D v((float)m_mouseX - w / 2, (float)m_mouseY - h / 2);
        return v;
    }



    /* WINDOW */

    bool Input::shouldClose() noexcept {
        return m_shouldClose;
    }

}