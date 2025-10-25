/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL2/SDL.h>

#include "engine/core/engine.typedef.hpp"

namespace NoxEngine {

    /**
     * @brief [UP] -> [PRESSED (inst.)] -> [DOWN] -> [RELEASED (inst.)] -> [UP] ...
     */
    enum class KeyState {
        UP,
        DOWN,
        PRESSED,
        RELEASED,
    };

    enum class WheelState {
        NONE,
        UP,
        DOWN,
    };

    class Input {
        public:
            explicit Input();
            ~Input() = default;

            bool shouldClose() noexcept;
            void updateEvents() noexcept;

            bool isKeyDown(const SDL_Scancode key) const noexcept;
            bool isKeyUp(const SDL_Scancode key) const noexcept;
            bool isKeyPressed(const SDL_Scancode key) const noexcept;
            bool isKeyReleased(const SDL_Scancode key) const noexcept;
            
            bool isMouseButtonDown(const Uint8 button) const noexcept;
            bool isMouseButtonUp(const Uint8 button) const noexcept;
            bool isMouseButtonPressed(const Uint8 button) const noexcept;
            bool isMouseButtonReleased(const Uint8 button) const noexcept;
            bool isMouseMoving() const noexcept;

            int getMouseX() const noexcept;
            int getMouseY() const noexcept;
            WheelState wheelScroll() const noexcept;
            V2D getMouseMovement() const noexcept;
            V2D getMousePosition() const noexcept;
            V2D getMousePointFromCenter(SDL_Window* window) const noexcept;

        private:
            void update() noexcept;
            void finalizeLastFrameState() noexcept;

            KeyState m_keys[SDL_NUM_SCANCODES] {};
            KeyState m_mouseButtons[8] {};
            WheelState m_wheel { WheelState::NONE };
            int m_mouseX {0}, m_mouseY {0}, m_mouseRelX {0}, m_mouseRelY {0};
            int m_oldMouseX {0}, m_oldMouseY {0};
            V2D m_mouseMotion { 0.0f, 0.0f };
            bool m_shouldClose { false };
    };

}

#endif // INPUT_HPP