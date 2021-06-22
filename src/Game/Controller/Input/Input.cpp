#include "Input.h"

#include <iostream>

Input::Input():
    mouseX(0), mouseY(0), mouseRelX(0), mouseRelY(0),
    oldMouseX(0), oldMouseY(0), oldMouseRelX(0), oldMouseRelY(0),
    close(false)
{
    for (int i=0; i < SDL_NUM_SCANCODES; i++)
		keys[i] = false;

	for (int i=0; i < 8; i++)
		mouseButtons[i] = false;

	wheelEvent = 0;
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
                    close = true;
                    break;

            case SDL_KEYDOWN:
                if(!keys[kc])
                    keys[kc] = true;
			    break;

			// case of key released
            case SDL_KEYUP:
                if(keys[kc])
                    keys[kc] = false;
                break;

                // ----------------------------- MOUSE
                // mouse button pressed
            case SDL_MOUSEBUTTONDOWN:
                if(!mouseButtons[mb])
                    mouseButtons[mb] = true;
                break;

                // mouse button released
            case SDL_MOUSEBUTTONUP:
                if(mouseButtons[mb])
                    mouseButtons[mb] = false;
                break;

                // mouse move
            case SDL_MOUSEMOTION:
                oldMouseX = mouseX;
                oldMouseY = mouseY;
                oldMouseRelX = mouseRelX;
                oldMouseRelY = mouseRelY;
                
                mouseX = events.motion.x;
                mouseY = events.motion.y;

                mouseRelX = events.motion.xrel;
                mouseRelY = events.motion.yrel;
                break;
        }

        wheelEvent = (events.wheel.y > 0)? 1 : (events.wheel.y < 0)? -1 : 0;
    }
}

bool Input::shouldClose() const {
    return close;
}

// SHOW OR HIDE POINTER
void Input::showPointer(bool show) const {
	if(show) SDL_ShowCursor(SDL_ENABLE);
	else SDL_ShowCursor(SDL_DISABLE);
}

// FIX THE POINTER TO CENTER CENTER OF THE WINDOW
void Input::capturePointer(bool capture) const {
	if(capture) SDL_SetRelativeMouseMode(SDL_TRUE);
	else SDL_SetRelativeMouseMode(SDL_FALSE);
}

bool Input::isKeyDown(const SDL_Scancode key) const {
    return keys[key];
}

bool Input::isMouseButtonDown(const Uint8 button) const {
	return mouseButtons[button];
}

bool Input::isMouseMoving() const {
    glm::vec2 v = getMouseDir();
	return v.x != 0 && v.y != 0;
}

int Input::getMouseX() const {
	return mouseX;
}

// actual Y
int Input::getMouseY() const {
	return mouseY;
}

// older X
int Input::getRelMouseX() const {
	return mouseRelX;
}

// older Y
int Input::getRelMouseY() const {
	return mouseRelY;
}

int Input::wheelScroll() const {
	return wheelEvent;
}

glm::vec2 Input::getMouseDir() const {
    return glm::vec2(mouseRelX, mouseRelY);
}