#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL2/SDL.h>

#include "engine.typedef.hpp"

namespace NoxEngine {

    class Input {
        public:
            Input();
            ~Input();

            bool shouldClose();
            void updateEvents();

            bool isKeyDown(const SDL_Scancode key) const;
            bool isMouseButtonDown(const Uint8 button) const;
            bool isMouseMoving() const;

            int getMouseX() const;
            int getMouseY() const;
            int wheelScroll() const;
            V2D getMouseDir() const;
            V2D getMousePoint() const;
            V2D getMousePointFromCenter(SDL_Window* window) const;

        private:
            bool m_keys[SDL_NUM_SCANCODES];
            bool m_mouseButtons[8];
            int m_wheelEvent;
            int m_mouseX, m_mouseY, m_mouseRelX, m_mouseRelY;
            int m_oldMouseX, m_oldMouseY;
            V2D mouseMotion;
            bool m_shouldClose;
    };

}

#endif // INPUT_HPP