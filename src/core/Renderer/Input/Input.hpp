#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL3/SDL.h>

#include "core/engine.typedef.hpp"

namespace NoxEngine {

    class Input {
        public:
            explicit Input();
            ~Input();

            bool shouldClose() noexcept;
            void updateEvents() noexcept;

            bool isKeyDown(const SDL_Scancode key) const noexcept;
            bool isMouseButtonDown(const Uint8 button) const noexcept;
            bool isMouseMoving() const noexcept;

            int getMouseX() const noexcept;
            int getMouseY() const noexcept;
            int wheelScroll() const noexcept;
            V2D getMouseDir() const noexcept;
            V2D getMousePoint() const noexcept;
            V2D getMousePointFromCenter(SDL_Window* window) const noexcept;

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