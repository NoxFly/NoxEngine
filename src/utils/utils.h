#ifndef UTILS_H
#define UTILS_H

#include <string_view>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

int inInterval(int min, int val, int max);

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
        b = inInterval(0, alpha, 255);
    }

    Color_t(const Color_t& color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
    }

    friend ostream& operator<<(ostream& os, const Color_t& color) {
        os << "{ r: " << color.r << ", g: " << color.g << ", b: " << color.b << ", a: " << color.a << " }";
        return os;
    }
} Color;

bool endsWith(string_view str, string_view suffix);
bool startsWith(string_view str, string_view prefix);
bool isNumber(const string& str);
string replace(const string& str, const string& find, const string& replace);
vector<string> split(string sequence, string separator);
Color getColorFromString(string str);

#endif // UTILS_H