#ifndef UTILS_HPP
#define UTILS_HPP

#include <string_view>
#include <string>
#include <vector>
#include <iostream>

#include "Color.hpp"


/**
 * Returns either a suffix is at the end of a string or not
 * @param str The parent string to search the suffix in
 * @param suffix The suffix to search
 * @return Either the string has the suffix or not
 */
bool endsWith(std::string_view str, std::string_view suffix);

/**
 * Returns either a prefix is at the start of a string or not
 * @param str The parent string to search the prefix in
 * @param prefix The preefix to search
 * @return Either the string has the prefix or not
 */
bool startsWith(std::string_view str, std::string_view prefix);

/**
 * Returns either a given string is an integer or not
 * @param str The string to check
 * @return Either the string can be converted to an integer or not
 */
bool isInteger(const std::string& str);

/**
 * Returns either a given string is a float or not
 * @param str The string to check
 * @return Either the string can be converted to a float or not
 */
bool isFloat(const std::string& str);

/**
 * Replaces a substring in the given string
 * @param str The main string
 * @param find The substring to replace
 * @param replace The substring to replace by
 * @return The modified string
 */
std::string replace(const std::string& str, const std::string& find, const std::string& replace);

/**
 * Splits the given string in vector of substrings, delimited by the given separator
 * @param sequence The string to split
 * @param separator The delimiter
 * @return The vector of splitted substrings
 */
std::vector<std::string> split(std::string sequence, std::string separator);

/**
 * Reads a string and tries to convert it to a RGB color struct.
 * If it can't convert, then it returns black by default.
 * @param str The string to read and convert
 * @return The converted color
 */
Color getColorFromString(std::string str);

std::string generateUUID();

#endif // UTILS_HPP