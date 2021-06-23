#include "Input.h"

#include <iostream>

Input::Input():
    m_mouseX(0), m_mouseY(0), m_mouseRelX(0), m_mouseRelY(0),
    m_oldMouseX(0), m_oldMouseY(0), m_oldMouseRelX(0), m_oldMouseRelY(0),
    m_close(false)
{
    for (int i=0; i < SDL_NUM_SCANCODES; i++)
		m_keys[i] = false;

	for (int i=0; i < 8; i++)
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
                    m_close = true;
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
                m_oldMouseRelX = m_mouseRelX;
                m_oldMouseRelY = m_mouseRelY;
                
                m_mouseX = events.motion.x;
                m_mouseY = events.motion.y;

                m_mouseRelX = events.motion.xrel;
                m_mouseRelY = events.motion.yrel;
                break;
        }

        // wheel
        m_wheelEvent = (events.wheel.y > 0)? 1 : (events.wheel.y < 0)? -1 : 0;
    }
}

bool Input::shouldClose() const {
    return m_close;
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

// older X
int Input::getRelMouseX() const {
	return m_mouseRelX;
}

// older Y
int Input::getRelMouseY() const {
	return m_mouseRelY;
}

int Input::wheelScroll() const {
	return m_wheelEvent;
}

glm::vec2 Input::getMouseDir() const {
    return glm::vec2(m_mouseRelX, m_mouseRelY);
}