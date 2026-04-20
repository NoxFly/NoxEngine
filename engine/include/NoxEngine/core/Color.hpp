// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <cstdint>

namespace Nox {

    struct Color {
        float r = 1.0f;
        float g = 1.0f;
        float b = 1.0f;
        float a = 1.0f;

        constexpr Color() = default;
        constexpr Color(float r, float g, float b, float a = 1.0f)
            : r(r), g(g), b(b), a(a) {}

        static const Color White;
        static const Color Black;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
    };

    inline constexpr Color Color::White { 1.0f, 1.0f, 1.0f, 1.0f };
    inline constexpr Color Color::Black { 0.0f, 0.0f, 0.0f, 1.0f };
    inline constexpr Color Color::Red   { 1.0f, 0.0f, 0.0f, 1.0f };
    inline constexpr Color Color::Green { 0.0f, 1.0f, 0.0f, 1.0f };
    inline constexpr Color Color::Blue  { 0.0f, 0.0f, 1.0f, 1.0f };

} // namespace Nox
