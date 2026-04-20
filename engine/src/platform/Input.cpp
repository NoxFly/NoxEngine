// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/platform/Input.hpp>

#include <SDL3/SDL.h>

namespace Nox {

    void Input::processEvent(const SDL_Event& event) {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN: {
                if (!event.key.repeat) {
                    auto k = static_cast<int32_t>(event.key.key);
                    keysDown_.insert(k);
                    keysPressed_.insert(k);
                    onKeyDown.emit(static_cast<Key>(k));
                }
                break;
            }
            case SDL_EVENT_KEY_UP: {
                auto k = static_cast<int32_t>(event.key.key);
                keysDown_.erase(k);
                keysReleased_.insert(k);
                onKeyUp.emit(static_cast<Key>(k));
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                uint32_t mask = 1u << event.button.button;
                mouseButtonsDown_ |= mask;
                mouseButtonsPressed_ |= mask;
                onMouseDown.emit(static_cast<MouseButton>(event.button.button));
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                uint32_t mask = 1u << event.button.button;
                mouseButtonsDown_ &= ~mask;
                mouseButtonsReleased_ |= mask;
                onMouseUp.emit(static_cast<MouseButton>(event.button.button));
                break;
            }
            case SDL_EVENT_MOUSE_MOTION: {
                mousePos_ = { event.motion.x, event.motion.y };
                mouseDelta_ += Math::Vec2{ event.motion.xrel, event.motion.yrel };
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL: {
                scrollDelta_ += event.wheel.y;
                onScroll.emit(event.wheel.y);
                break;
            }
            default:
                break;
        }
    }

    void Input::endFrame() {
        keysPressed_.clear();
        keysReleased_.clear();
        mouseButtonsPressed_ = 0;
        mouseButtonsReleased_ = 0;
        mouseDelta_ = { 0.0f, 0.0f };
        scrollDelta_ = 0.0f;
    }

    bool Input::isKeyDown(Key key) const {
        return keysDown_.contains(static_cast<int32_t>(key));
    }

    bool Input::isKeyPressed(Key key) const {
        return keysPressed_.contains(static_cast<int32_t>(key));
    }

    bool Input::isKeyReleased(Key key) const {
        return keysReleased_.contains(static_cast<int32_t>(key));
    }

    bool Input::isMouseButtonDown(MouseButton btn) const {
        return (mouseButtonsDown_ & (1u << static_cast<uint8_t>(btn))) != 0;
    }

    bool Input::isMouseButtonPressed(MouseButton btn) const {
        return (mouseButtonsPressed_ & (1u << static_cast<uint8_t>(btn))) != 0;
    }

    bool Input::isMouseButtonReleased(MouseButton btn) const {
        return (mouseButtonsReleased_ & (1u << static_cast<uint8_t>(btn))) != 0;
    }

} // namespace Nox
