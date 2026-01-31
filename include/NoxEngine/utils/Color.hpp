/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef COLOR_HPP
#define COLOR_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

/**
 * @struct A basic RGBA struct
 */
struct Color {
    float r, g, b, a=1;

    Color(): Color(0.0f, 0.0f, 0.0f, 0.0f)
    {}

    Color(const int red, const int green, const int blue):
        Color(red, green, blue, 255)
    {}

    Color(const int red, const int green, const int blue, const int alpha) {
        r = std::clamp(red,     0, 255) / 255.f;
        g = std::clamp(green,   0, 255) / 255.f;
        b = std::clamp(blue,    0, 255) / 255.f;
        a = std::clamp(alpha,   0, 255) / 255.f;
    }

    Color(const float red, const float green, const float blue):
        Color(red, green, blue, 1.0f)
    {}

    Color(const float red, const float green, const float blue, const float alpha) {
        r = std::clamp(red,     0.f, 1.0f);
        g = std::clamp(green,   0.f, 1.0f);
        b = std::clamp(blue,    0.f, 1.0f);
        a = std::clamp(alpha,   0.f, 1.0f);
    }

    Color(const Color& color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
    }

    Color& operator=(const Color& color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;

        return *this;
    }

    glm::vec3 vec3() const noexcept {
        return glm::vec3(r, g, b);
    }

    glm::vec4 vec4() const noexcept {
        return glm::vec4(r, g, b, a);
    }

    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        std::string r = std::to_string(color.r),
            g = std::to_string(color.g),
            b = std::to_string(color.b),
            a = std::to_string(color.a);

        std::string str = "{ r: " + r + ", g: " + g + ", b: " + b + ", a: " + a + " }";
        os << str;
        return os;
    }
};

#endif // COLOR_HPP