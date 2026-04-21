// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/platform/InputMapping.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <SDL3/SDL.h>

namespace Nox {

    namespace {
        /// Detect keyboard layout from the OS via SDL.
        /// Heuristic: check whether the physical key at scancode Q produces 'a'
        /// (AZERTY) or 'q' (QWERTY).
        [[nodiscard]] KeyboardLayout detectKeyboardLayout() {
            SDL_Keycode keyForQ = SDL_GetKeyFromScancode(SDL_SCANCODE_Q, SDL_KMOD_NONE, false);

            if (keyForQ == SDLK_A) {
                return KeyboardLayout::AZERTY;
            }
            return KeyboardLayout::QWERTY;
        }
    }

    InputMapping::InputMapping() {
        detectLayout();
    }

    void InputMapping::detectLayout() {
        layout_ = detectKeyboardLayout();
        setDefaults(layout_);
        NOX_LOG_INFO("Keyboard layout detected: {}",
                     layout_ == KeyboardLayout::AZERTY ? "AZERTY" : "QWERTY");
    }

    void InputMapping::setLayout(KeyboardLayout layout) {
        layout_ = layout;
        setDefaults(layout);
    }

    void InputMapping::setDefaults(KeyboardLayout layout) {
        bindings_.clear();

        switch (layout) {
            case KeyboardLayout::AZERTY:
                bindings_[Action::MoveForward]  = Key::Z;
                bindings_[Action::MoveBackward] = Key::S;
                bindings_[Action::MoveLeft]     = Key::Q;
                bindings_[Action::MoveRight]    = Key::D;
                bindings_[Action::MoveUp]       = Key::Space;
                bindings_[Action::MoveDown]     = Key::LShift;
                break;

            case KeyboardLayout::QWERTY:
                [[fallthrough]];
            default:
                bindings_[Action::MoveForward]  = Key::W;
                bindings_[Action::MoveBackward] = Key::S;
                bindings_[Action::MoveLeft]     = Key::A;
                bindings_[Action::MoveRight]    = Key::D;
                bindings_[Action::MoveUp]       = Key::Space;
                bindings_[Action::MoveDown]     = Key::LShift;
                break;
        }
    }

    void InputMapping::bind(Action action, Key key) {
        bindings_[action] = key;
    }

    bool InputMapping::isActionDown(const Input& input, Action action) const {
        auto it = bindings_.find(action);
        if (it == bindings_.end()) { return false; }
        return input.isKeyDown(it->second);
    }

    bool InputMapping::isActionPressed(const Input& input, Action action) const {
        auto it = bindings_.find(action);
        if (it == bindings_.end()) { return false; }
        return input.isKeyPressed(it->second);
    }

    bool InputMapping::isActionReleased(const Input& input, Action action) const {
        auto it = bindings_.find(action);
        if (it == bindings_.end()) { return false; }
        return input.isKeyReleased(it->second);
    }

    Key InputMapping::keyFor(Action action) const {
        auto it = bindings_.find(action);
        if (it == bindings_.end()) { return Key::Unknown; }
        return it->second;
    }

} // namespace Nox
