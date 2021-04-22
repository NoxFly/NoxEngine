#include "Input.h"

// CONSTRUCTOR
Input::Input():
	m_x(0), m_y(0), m_xRel(0), m_yRel(0), m_closeStates{}
{
	for(int i(0); i < SDL_NUM_SCANCODES; i++)
		m_keys[i] = false;

	for(int i(0); i < 8; i++)
		m_mouseButtons[i] = false;

	m_wheelEvent = 0;
}

// DESTRUCTOR
Input::~Input() {

}

// UPDATE EVENTS
void Input::updateEvents() {
	m_xRel = 0;
	m_yRel = 0;

	// Event loop
	while(SDL_PollEvent(&m_events)) {
		switch(m_events.type) {
			// ----------------------------- KEYBOARD
			// case of key pressed
            case SDL_KEYDOWN:
                m_keys[m_events.key.keysym.scancode] = true;
                break;

            // case of key released
            case SDL_KEYUP:
                m_keys[m_events.key.keysym.scancode] = false;
                break;

            // ----------------------------- MOUSE
            // mouse button pressed
            case SDL_MOUSEBUTTONDOWN:
                m_mouseButtons[m_events.button.button] = true;
                break;

            // mouse button released
            case SDL_MOUSEBUTTONUP:
                m_mouseButtons[m_events.button.button] = false;
                break;

            // mouse move
            case SDL_MOUSEMOTION:
                m_x = m_events.motion.x;
                m_y = m_events.motion.y;

                m_xRel = m_events.motion.xrel;
                m_yRel = m_events.motion.yrel;
                break;

            case SDL_WINDOWEVENT:
                if(m_events.window.event == SDL_WINDOWEVENT_CLOSE)
                    m_closeStates[m_events.window.windowID] = true;
                break;

            default:
                break;
		}

		// SCROLLING UP/DOWN 120 = UP -120 = DOWN
		if(m_events.wheel.y > 0)
			m_wheelEvent = 1;
		else if(m_events.wheel.y < 0)
			m_wheelEvent = -1;
		else
			m_wheelEvent = 0;
	}
}

// getters and setters

bool Input::shouldClose(SDL_Window* window) const {
	return shouldClose(SDL_GetWindowID(window));
}

bool Input::shouldClose(Uint32 windowId) const {
    return m_closeStates.count(windowId) && m_closeStates.at(windowId) == true;
}

bool Input::getKey(const SDL_Scancode key) const {
	return m_keys[key];
}

bool Input::getMouseButton(const Uint8 button) const {
	return m_mouseButtons[button];
}

bool Input::mouseMovement() const {
    return !(m_xRel == 0 && m_yRel == 0);
}

// Getters of mouse position
// actual X
int Input::getX() const {
	return m_x;
}

// actual Y
int Input::getY() const {
	return m_y;
}

// older X
int Input::getXRel() const {
	return m_xRel;
}

// older Y
int Input::getYRel() const {
	return m_yRel;
}

int Input::wheelScroll() const {
	return m_wheelEvent;
}

// reset wheel event to not scrolling every frame
void Input::resetWheelEvent() {
	m_wheelEvent = 0;
}

// MOVEMENT KEY PRESSED
bool Input::getMovementKeyPressed() const {
	// if it's one of the displacement key, return true else false
	return (
        m_keys[SDL_SCANCODE_UP]     || m_keys[SDL_SCANCODE_DOWN]    || m_keys[SDL_SCANCODE_LEFT]    || m_keys[SDL_SCANCODE_RIGHT]
    ||  m_keys[SDL_SCANCODE_W]      || m_keys[SDL_SCANCODE_S]       || m_keys[SDL_SCANCODE_A]       || m_keys[SDL_SCANCODE_D]
    ||  m_keys[SDL_SCANCODE_SPACE]  || m_keys[SDL_SCANCODE_LCTRL]
    );
}

// SHOW OR HIDE POINTER
void Input::showPointer(bool response) const {
	if(response) SDL_ShowCursor(SDL_ENABLE);
	else SDL_ShowCursor(SDL_DISABLE);
}

// FIX THE POINTER TO CENTER CENTER OF THE WINDOW
void Input::capturePointer(bool response) const {
	if(response) SDL_SetRelativeMouseMode(SDL_TRUE);
	else SDL_SetRelativeMouseMode(SDL_FALSE);
}