#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <string>

#define CONSOLE_END_COLOR "\033[0m"
#define CONSOLE_RED     "\033[0;31m"
#define CONSOLE_GREEN   "\033[0;32m"
#define CONSOLE_YELLOW  "\033[0;33m"
#define CONSOLE_BLUE    "\033[0;34m"
#define CONSOLE_PURPLE  "\033[0;35m"
#define CONSOLE_CYAN    "\033[0;36m"
#define CONSOLE_WHITE   "\033[0;37m"

class Console {
	public:
        static void log(std::string where, std::string message);
        static void info(std::string where, std::string message);
        static void warn(std::string where, std::string message);
        static void error(std::string where, std::string message);

    protected:
        enum method { LOG, INFO, WARN, ERROR };

        static std::string methodName(Console::method method);
        static std::string coloredName(Console::method method);
        static void message(Console::method method, std::string where, std::string message);
};

#endif // CONSOLE_H