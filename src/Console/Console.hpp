/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <iostream>
#include <string>

#ifdef LINUX
#define CONSOLE_END_COLOR   "\033[0m"
#define CONSOLE_RED         "\033[0;31m"
#define CONSOLE_GREEN       "\033[0;32m"
#define CONSOLE_YELLOW      "\033[0;33m"
#define CONSOLE_BLUE        "\033[0;34m"
#define CONSOLE_PURPLE      "\033[0;35m"
#define CONSOLE_CYAN        "\033[0;36m"
#define CONSOLE_WHITE       "\033[0;37m"
#else
#define CONSOLE_END_COLOR   ""
#define CONSOLE_RED         ""
#define CONSOLE_GREEN       ""
#define CONSOLE_YELLOW      ""
#define CONSOLE_BLUE        ""
#define CONSOLE_PURPLE      ""
#define CONSOLE_CYAN        ""
#define CONSOLE_WHITE       ""
#endif

class Console {
	public:
        static void log(const std::string& message);
        static void info(const std::string& message);
        static void warn(const std::string& message);
        static void error(const std::string& message);

        static void log(const std::string& where, const std::string& message);
        static void info(const std::string& where, const std::string& message);
        static void warn(const std::string& where, const std::string& message);
        static void error(const std::string& where, const std::string& message);

    protected:
        enum method { LOG, INFO, WARN, ERR };

        static std::string methodName(Console::method method);
        static std::string coloredName(Console::method method);
        static void message(Console::method method, const std::string& where, const std::string& message);
};

#endif // CONSOLE_HPP