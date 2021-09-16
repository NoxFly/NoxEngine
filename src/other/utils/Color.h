#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <string>


/**
 * Returns the value if it's in the given interval, or one of the given bound if it overflows
 * @param min The left side interval bound
 * @param val The value to return and check if it's in the interval
 * @param max The right side interval bound
 * @return The mapped value
 */
/* int inInterval(int min, int val, int max) {
    return std::max(min, std::min(val, max));
} */

/**
 * @struct A basic RGB struct
 */
typedef struct Color_t {
    int r;
    int g;
    int b;
    int a=255;

    Color_t() {
        Color_t(0, 0, 0, 0);
    }

    Color_t(int red, int green, int blue) {
        Color_t(red, green, blue, 255);
    }

    Color_t(int red, int green, int blue, int alpha) {
        r = std::max(0, std::min(red, 255));
        g = std::max(0, std::min(green, 255));
        b = std::max(0, std::min(blue, 255));
        a = std::max(0, std::min(alpha, 255));
    }

    Color_t(const Color_t& color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
    }

    Color_t& operator=(const Color_t& color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Color_t& color) {
        std::string r = std::to_string(color.r),
            g = std::to_string(color.g),
            b = std::to_string(color.b),
            a = std::to_string(color.a);

        std::string str = "{ r: " + r + ", g: " + g + ", b: " + b + ", a: " + a + " }";
        os << str;
        return os;
    }
} Color;

#endif // COLOR_H