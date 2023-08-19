#include "Input.hpp"

#include <iostream>
#include <glm/gtx/transform.hpp>


namespace NoxEngine {

    Input::Input():
        m_mouseX(0), m_mouseY(0), m_mouseRelX(0), m_mouseRelY(0),
        m_oldMouseX(0), m_oldMouseY(0), mouseMotion(0, 0),
        m_shouldClose(false)
    {
        for(int i=0; i < SDL_NUM_SCANCODES; i++)
            m_keys[i] = false;

        for(int i=0; i < 8; i++)
            m_mouseButtons[i] = false;

        m_wheelEvent = 0;
    }

    Input::~Input() {

    }


    void Input::updateEvents() noexcept {
        SDL_Event events;

        while(SDL_PollEvent(&events)) {
            SDL_Scancode kc = events.key.keysym.scancode;
            Uint8 mb = events.button.button;

            switch(events.type) {
                case SDL_EventType::SDL_EVENT_QUIT:
                    m_shouldClose = true;
                    break;

                // ----------------------------- KEYBOARD
                // key down
                case SDL_EVENT_KEY_DOWN:
                    if(!m_keys[kc])
                        m_keys[kc] = true;
                    break;

                // key up
                case SDL_EVENT_KEY_UP:
                    if(m_keys[kc])
                        m_keys[kc] = false;
                    break;

                // ----------------------------- MOUSE
                // mouse button pressed
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if(!m_mouseButtons[mb])
                        m_mouseButtons[mb] = true;
                    break;

                // mouse button released
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if(m_mouseButtons[mb])
                        m_mouseButtons[mb] = false;
                    break;

                // mouse move
                case SDL_EVENT_MOUSE_MOTION:
                    m_oldMouseX = m_mouseX;
                    m_oldMouseY = m_mouseY;

                    mouseMotion = V2D(events.motion.xrel, events.motion.yrel);
                    
                    m_mouseX = events.button.x;
                    m_mouseY = events.button.y;
                    break;
            }

            // wheel
            m_wheelEvent = (events.wheel.y > 0)? 1 : (events.wheel.y < 0)? -1 : 0;
        }
    }


    bool Input::isKeyDown(const SDL_Scancode key) const noexcept {
        return m_keys[key];
    }

    bool Input::isMouseButtonDown(const Uint8 button) const noexcept {
        return m_mouseButtons[button];
    }

    bool Input::isMouseMoving() const noexcept {
        V2D v = getMouseDir();
        return v.x != 0 && v.y != 0;
    }

    int Input::getMouseX() const noexcept {
        return m_mouseX;
    }

    // actual Y
    int Input::getMouseY() const noexcept {
        return m_mouseY;
    }

    int Input::wheelScroll() const noexcept {
        return m_wheelEvent;
    }

    V2D Input::getMouseDir() const noexcept {
        return mouseMotion;
    }

    V2D Input::getMousePoint() const noexcept {
        return V2D(m_mouseX, m_mouseY);
    }

    V2D Input::getMousePointFromCenter(SDL_Window* window) const noexcept {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        V2D v((float)m_mouseX - w / 2, (float)m_mouseY - h / 2);
        return v;
    }

    bool Input::shouldClose() noexcept {
        return m_shouldClose;
    }

}