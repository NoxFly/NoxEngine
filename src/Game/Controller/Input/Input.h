#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <glm/vec2.hpp>

class Input {
	public:
		Input();
		~Input();

        void updateEvents();
        bool shouldClose() const;

        void showPointer(bool response) const;
		void capturePointer(bool response) const;

        bool isKeyDown(const SDL_Scancode key) const;
		bool isMouseButtonDown(const Uint8 button) const;
		bool isMouseMoving() const;

        int getMouseX() const;
		int getMouseY() const;
		int getRelMouseX() const;
		int getRelMouseY() const;
		int wheelScroll() const;
        glm::vec2 getMouseDir() const;

    private:
        bool keys[SDL_NUM_SCANCODES];
		bool mouseButtons[8];
		int wheelEvent;
        int mouseX, mouseY, mouseRelX, mouseRelY;
        int oldMouseX, oldMouseY, oldMouseRelX, oldMouseRelY;
        bool close;
};

#endif // INPUT_H