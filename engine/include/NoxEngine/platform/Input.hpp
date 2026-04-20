// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/core/Signal.hpp>
#include <NoxEngine/math/Types.hpp>

#include <cstdint>
#include <unordered_set>

union SDL_Event;

namespace Nox {

    enum class Key : int32_t {
        Unknown = 0,
        A = 'a', B = 'b', C = 'c', D = 'd', E = 'e', F = 'f',
        G = 'g', H = 'h', I = 'i', J = 'j', K = 'k', L = 'l',
        M = 'm', N = 'n', O = 'o', P = 'p', Q = 'q', R = 'r',
        S = 's', T = 't', U = 'u', V = 'v', W = 'w', X = 'x',
        Y = 'y', Z = 'z',
        Num0 = '0', Num1 = '1', Num2 = '2', Num3 = '3', Num4 = '4',
        Num5 = '5', Num6 = '6', Num7 = '7', Num8 = '8', Num9 = '9',
        Space = ' ', Escape = 27, Enter = 13, Tab = 9, Backspace = 8,
        Up = 0x40000052, Down = 0x40000051, Left = 0x40000050, Right = 0x4000004F,
        LShift = 0x400000E1, RShift = 0x400000E5,
        LCtrl = 0x400000E0, RCtrl = 0x400000E4,
        LAlt = 0x400000E2, RAlt = 0x400000E6,
        F1 = 0x4000003A, F2 = 0x4000003B, F3 = 0x4000003C, F4 = 0x4000003D,
        F5 = 0x4000003E, F6 = 0x4000003F, F7 = 0x40000040, F8 = 0x40000041,
        F9 = 0x40000042, F10 = 0x40000043, F11 = 0x40000044, F12 = 0x40000045,
        PageUp = 0x4000004B, PageDown = 0x4000004E,
    };

    enum class MouseButton : uint8_t {
        Left   = 1,
        Middle = 2,
        Right  = 3,
    };

    class Input {
    public:
        Input() = default;

        // Called by Window each frame
        void processEvent(const SDL_Event& event);
        void endFrame();

        // ── Keyboard ───────────────────────────────────────────────
        [[nodiscard]] bool isKeyDown(Key key) const;
        [[nodiscard]] bool isKeyPressed(Key key) const;
        [[nodiscard]] bool isKeyReleased(Key key) const;

        // ── Mouse ──────────────────────────────────────────────────
        [[nodiscard]] bool isMouseButtonDown(MouseButton btn) const;
        [[nodiscard]] bool isMouseButtonPressed(MouseButton btn) const;
        [[nodiscard]] bool isMouseButtonReleased(MouseButton btn) const;

        [[nodiscard]] Math::Vec2 mousePosition() const { return mousePos_; }
        [[nodiscard]] Math::Vec2 mouseDelta() const { return mouseDelta_; }
        [[nodiscard]] float scrollDelta() const { return scrollDelta_; }

        // ── Signals ────────────────────────────────────────────────
        Signal<Key>         onKeyDown;
        Signal<Key>         onKeyUp;
        Signal<MouseButton> onMouseDown;
        Signal<MouseButton> onMouseUp;
        Signal<float>       onScroll;

    private:
        std::unordered_set<int32_t> keysDown_;
        std::unordered_set<int32_t> keysPressed_;
        std::unordered_set<int32_t> keysReleased_;

        uint32_t mouseButtonsDown_     = 0;
        uint32_t mouseButtonsPressed_  = 0;
        uint32_t mouseButtonsReleased_ = 0;

        Math::Vec2 mousePos_{ 0.0f };
        Math::Vec2 mouseDelta_{ 0.0f };
        float      scrollDelta_ = 0.0f;
    };

} // namespace Nox
