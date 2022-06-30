#include "Console.hpp"

#include <thread>
#include <mutex>

std::mutex consoleMutex;

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

void Console::message(Console::method method, const std::string& where, const std::string& message) {
    std::string name = Console::methodName(method);
    std::ostream &stream = (name.compare("ERROR"))? std::cout : std::cerr;

    std::string spaces = " ";
    std::string sMethod = coloredName(method);

    int sSize = sMethod.size();

    for(int i=10; i > sSize; i--) {
        spaces += " ";
    }

    std::string sep = (where == "")? " " : " : ";

    std::lock_guard<std::mutex> guard(consoleMutex);
    stream << sMethod << spaces << where << sep << message << std::endl;
}

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