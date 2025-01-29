/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef COLOR_HPP
#define COLOR_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <glm/vec4.hpp>

/**
 * @struct A basic RGBA struct
 */
struct Color {
    float r, g, b, a=1;

    Color() {
        Color(0, 0, 0, 0);
    }

    Color(const unsigned short red, const unsigned short green, const unsigned short blue):
        Color(red, green, blue, 255)
    {
        
    }

    Color(const unsigned short red, const unsigned short green, const unsigned short blue, const unsigned short alpha) {
        r = std::clamp(red,     (unsigned short)0, (unsigned short)255) / 255.f;
        g = std::clamp(green,   (unsigned short)0, (unsigned short)255) / 255.f;
        b = std::clamp(blue,    (unsigned short)0, (unsigned short)255) / 255.f;
        a = std::clamp(alpha,   (unsigned short)0, (unsigned short)255) / 255.f;
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