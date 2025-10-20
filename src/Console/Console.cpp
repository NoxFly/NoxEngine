/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "Console.hpp"

#include <thread>
#include <mutex>
#include <string>
#include <chrono>

std::mutex consoleMutex;

std::string getCurrentTimestamp();

void Console::log(const std::string& where, const std::string& message) {
    Console::message(LOG, where, message);
}

void Console::info(const std::string& where, const std::string& message) {
    Console::message(INFO, where, message);
}

void Console::warn(const std::string& where, const std::string& message) {
    Console::message(WARN, where, message);
}

void Console::error(const std::string& where, const std::string& message) {
    Console::message(ERR, where, message);
}

#ifdef DEBUG
void Console::message(Console::method method, const std::string& where, const std::string& message) {
    std::string name = Console::methodName(method);
    std::ostream &stream = (name.compare("ERROR"))? std::cout : std::cerr;

    std::string spaces = " ";
    std::string sMethod = coloredName(method);

    size_t sSize = sMethod.size();

    for(size_t i=10; i > sSize; i--) {
        spaces += " ";
    }

    std::string sep = (where == "")
        ? " "
        : " : ";

    std::string timestamp = "[" + getCurrentTimestamp() + "]";

    std::lock_guard<std::mutex> guard(consoleMutex);

    stream << timestamp << " " << sMethod << spaces << where << sep << message << std::endl;
}
#else
void Console::message(Console::method method, const std::string& where, const std::string& message) {
    (void)method;
    (void)where;
    (void)message;
}
#endif

void Console::log(const std::string& message) {
    Console::message(LOG, "", message);
}

void Console::info(const std::string& message) {
    Console::message(INFO, "", message);
}

void Console::warn(const std::string& message) {
    Console::message(WARN, "", message);
}

void Console::error(const std::string& message) {
    Console::message(ERR, "", message);
}


std::string Console::methodName(Console::method method) {
    switch(method) {
        case LOG:   return "Log";
        case INFO:  return "Info";
        case WARN:  return "Warn";
        case ERR: return "Error";
        default:    return ">";
    }
}

std::string Console::coloredName(Console::method method) {
    std::string str = "[";

    switch(method) {
        case LOG:   str += CONSOLE_WHITE     + (std::string)"Log"; break;
        case INFO:  str += CONSOLE_CYAN      + (std::string)"Info"; break;
        case WARN:  str += CONSOLE_YELLOW    + (std::string)"Warning"; break;
        case ERR: str += CONSOLE_RED       + (std::string)"Error"; break;
        default:    str += ">";
    }

    return str + CONSOLE_END_COLOR + "]";
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    char buffer[20];
    std::tm timeinfo;
#ifdef _WIN32
    localtime_s(&timeinfo, &now_time_t);
#else
    localtime_r(&now_time_t, &timeinfo);
#endif
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);

    return std::string(buffer);
}