#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <string>
#include <algorithm>

/**
 * @struct A basic RGBA struct
 */
struct Color {
    int r, g, b, a=255;

    Color() {
        Color(0, 0, 0, 0);
    }

    Color(const int red, const int green, const int blue):
        Color(red, green, blue, 255)
    {
        
    }

    Color(const int red, const int green, const int blue, const int alpha) {
        r = std::clamp(red, 0, 255);
        g = std::clamp(green, 0, 255);
        b = std::clamp(blue, 0, 255);
        a = std::clamp(alpha, 0, 255);
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

#endif // COLOR_H