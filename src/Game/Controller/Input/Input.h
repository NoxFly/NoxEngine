#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <glm/vec2.hpp>

#include "Video.h"

class Input {
	public:
		Input();
		~Input();

        void setCurrentWindow(Video& window);
        void updateEvents();
        bool shouldClose() const;

        bool isKeyDown(const SDL_Scancode key) const;
		bool isMouseButtonDown(const Uint8 button) const;
		bool isMouseMoving() const;

        int getMouseX() const;
		int getMouseY() const;
		int wheelScroll() const;
        glm::vec2 getMouseDir() const;
        glm::vec2 getMousePoint() const;
        glm::vec2 getMousePointFromCenter() const;

    private:
        Video* m_window;
        bool m_keys[SDL_NUM_SCANCODES];
		bool m_mouseButtons[8];
		int m_wheelEvent;
        int m_mouseX, m_mouseY, m_mouseRelX, m_mouseRelY;
        int m_oldMouseX, m_oldMouseY;
        glm::vec2 mouseMotion;
        bool m_close;
};

#endif // INPUT_H