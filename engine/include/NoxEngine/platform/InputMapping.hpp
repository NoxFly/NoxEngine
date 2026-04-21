// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/platform/Input.hpp>

#include <cstdint>
#include <string_view>
#include <unordered_map>

namespace Nox {

    /// Logical actions that can be mapped to physical keys.
    enum class Action : uint8_t {
        MoveForward,
        MoveBackward,
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown,
    };

    /// Known keyboard layouts for default binding selection.
    enum class KeyboardLayout : uint8_t {
        QWERTY,   ///< US / UK — WASD
        AZERTY,   ///< French — ZQSD
    };

    /// Maps logical actions to physical keys.
    /// Provides keyboard-layout-aware defaults.
    class InputMapping {
    public:
        InputMapping();

        /// Re-detect the keyboard layout and reset defaults.
        void detectLayout();

        /// Set defaults for a specific layout (without auto-detection).
        void setLayout(KeyboardLayout layout);

        /// Bind a key to an action (overrides any previous binding).
        void bind(Action action, Key key);

        /// Query whether the key bound to the given action is currently held.
        [[nodiscard]] bool isActionDown(const Input& input, Action action) const;

        /// Query whether the key bound to the given action was just pressed.
        [[nodiscard]] bool isActionPressed(const Input& input, Action action) const;

        /// Query whether the key bound to the given action was just released.
        [[nodiscard]] bool isActionReleased(const Input& input, Action action) const;

        /// Get the key currently bound to an action.
        [[nodiscard]] Key keyFor(Action action) const;

        /// Get the detected keyboard layout.
        [[nodiscard]] KeyboardLayout layout() const { return layout_; }

    private:
        void setDefaults(KeyboardLayout layout);

        KeyboardLayout layout_ = KeyboardLayout::QWERTY;
        std::unordered_map<Action, Key> bindings_;
    };

} // namespace Nox
