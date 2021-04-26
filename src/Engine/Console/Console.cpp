#include "Console.h"

void Console::log(std::string where, std::string message) {
    Console::message(LOG, where, message);
}

void Console::info(std::string where, std::string message) {
    Console::message(INFO, where, message);
}

void Console::warn(std::string where, std::string message) {
    Console::message(WARN, where, message);
}

void Console::error(std::string where, std::string message) {
    Console::message(ERROR, where, message);
}

void Console::message(Console::method method, std::string where, std::string message) {
    std::string name = Console::methodName(method);
    std::ostream &stream = (name.compare("ERROR"))? std::cout : std::cerr;

    std::string spaces = " ";
    std::string sMethod = coloredName(method);

    for(int i=20; i > sMethod.size(); i--) {
        spaces += " ";
    }

    stream << sMethod << spaces << where << " : " << message << std::endl;
}

std::string Console::methodName(Console::method method) {
    switch(method) {
        case LOG:   return "Log";
        case INFO:  return "Info";
        case WARN:  return "Warn";
        case ERROR: return "Error";
        default:    return ">";
    }
}

std::string Console::coloredName(Console::method method) {
    std::string str = "[";

    switch(method) {
        case LOG:   str += CONSOLE_WHITE     + (std::string)"Log"; break;
        case INFO:  str += CONSOLE_CYAN      + (std::string)"Info"; break;
        case WARN:  str += CONSOLE_YELLOW    + (std::string)"Warning"; break;
        case ERROR: str += CONSOLE_RED       + (std::string)"Error"; break;
        default:    str += ">";
    }

    return str + CONSOLE_END_COLOR + "]";
}