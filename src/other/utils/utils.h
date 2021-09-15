#ifndef UTILS_H
#define UTILS_H

#include <string_view>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

/**
 * Returns the value if it's in the given interval, or one of the given bound if it overflows
 * @param min The left side interval bound
 * @param val The value to return and check if it's in the interval
 * @param max The right side interval bound
 * @return The mapped value
 */
int inInterval(int min, int val, int max);

/**
 * @struct A basic RGB struct
 */
typedef struct Color_t {
    int r;
    int g;
    int b;
    int a=255;

    Color_t(int red, int green, int blue) {
        Color_t(red, green, blue, 255);
    }

    Color_t(int red, int green, int blue, int alpha) {
        r = inInterval(0, red, 255);
        g = inInterval(0, green, 255);
        b = inInterval(0, blue, 255);
        a = inInterval(0, alpha, 255);
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

    friend ostream& operator<<(ostream& os, const Color_t& color) {
        std::string r = std::to_string(color.r),
            g = std::to_string(color.g),
            b = std::to_string(color.b),
            a = std::to_string(color.a);

        std::string str = "{ r: " + r + ", g: " + g + ", b: " + b + ", a: " + a + " }";
        os << str;
        return os;
    }
} Color;

/**
 * Returns either a suffix is at the end of a string or not
 * @param str The parent string to search the suffix in
 * @param suffix The suffix to search
 * @return Either the string has the suffix or not
 */
bool endsWith(string_view str, string_view suffix);

/**
 * Returns either a prefix is at the start of a string or not
 * @param str The parent string to search the prefix in
 * @param prefix The preefix to search
 * @return Either the string has the prefix or not
 */
bool startsWith(string_view str, string_view prefix);

/**
 * Returns either a given string is an integer or not
 * @param str The string to check
 * @return Either the string can be converted to an integer or not
 */
bool isInteger(const string& str);

/**
 * Returns either a given string is a float or not
 * @param str The string to check
 * @return Either the string can be converted to a float or not
 */
bool isFloat(const string& str);

/**
 * Replaces a substring in the given string
 * @param str The main string
 * @param find The substring to replace
 * @param replace The substring to replace by
 * @return The modified string
 */
string replace(const string& str, const string& find, const string& replace);

/**
 * Splits the given string in vector of substrings, delimited by the given separator
 * @param sequence The string to split
 * @param separator The delimiter
 * @return The vector of splitted substrings
 */
vector<string> split(string sequence, string separator);

/**
 * Reads a string and tries to convert it to a RGB color struct.
 * If it can't convert, then it returns black by default.
 * @param str The string to read and convert
 * @return The converted color
 */
Color getColorFromString(string str);

std::string generateUUID();

#endif // UTILS_H