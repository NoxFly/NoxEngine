#include "Input.h"

#include <iostream>
#include <glm/gtx/transform.hpp>


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


void Input::updateEvents() {
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
                if(!m_keys[kc])
                    m_keys[kc] = true;
			    break;

			// key up
            case SDL_KEYUP:
                if(m_keys[kc])
                    m_keys[kc] = false;
                break;

            // ----------------------------- MOUSE
            // mouse button pressed
            case SDL_MOUSEBUTTONDOWN:
                if(!m_mouseButtons[mb])
                    m_mouseButtons[mb] = true;
                break;

            // mouse button released
            case SDL_MOUSEBUTTONUP:
                if(m_mouseButtons[mb])
                    m_mouseButtons[mb] = false;
                break;

            // mouse move
            case SDL_MOUSEMOTION:
                m_oldMouseX = m_mouseX;
                m_oldMouseY = m_mouseY;

                mouseMotion = glm::vec2(events.motion.xrel, events.motion.yrel);
                
                m_mouseX = events.button.x;
                m_mouseY = events.button.y;
                break;
        }

        // wheel
        m_wheelEvent = (events.wheel.y > 0)? 1 : (events.wheel.y < 0)? -1 : 0;
    }
}


bool Input::isKeyDown(const SDL_Scancode key) const {
    return m_keys[key];
}

bool Input::isMouseButtonDown(const Uint8 button) const {
	return m_mouseButtons[button];
}

bool Input::isMouseMoving() const {
    glm::vec2 v = getMouseDir();
	return v.x != 0 && v.y != 0;
}

int Input::getMouseX() const {
	return m_mouseX;
}

// actual Y
int Input::getMouseY() const {
	return m_mouseY;
}

int Input::wheelScroll() const {
	return m_wheelEvent;
}

glm::vec2 Input::getMouseDir() const {
    return mouseMotion;
}

glm::vec2 Input::getMousePoint() const {
    return glm::vec2(m_mouseX, m_mouseY);
}

glm::vec2 Input::getMousePointFromCenter(SDL_Window* window) const {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    glm::vec2 v((float)m_mouseX - w / 2, (float)m_mouseY - h / 2);
    return v;
}

bool Input::shouldClose() {
    return m_shouldClose;
}