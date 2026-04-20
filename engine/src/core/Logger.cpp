// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/core/Logger.hpp>

#include <iostream>
#include <mutex>

namespace Nox::detail {

    namespace {

        std::mutex& logMutex() {
            static std::mutex mtx;
            return mtx;
        }

        constexpr std::string_view levelToString(LogLevel level) {
            switch (level) {
                case LogLevel::Trace: return "TRACE";
                case LogLevel::Info:  return "INFO ";
                case LogLevel::Warn:  return "WARN ";
                case LogLevel::Error: return "ERROR";
                case LogLevel::Fatal: return "FATAL";
            }
            return "?????";
        }

    } // anonymous namespace

    void logMessage(LogLevel level, std::string_view message) {
        const std::lock_guard lock(logMutex());

        auto& stream = (level >= LogLevel::Error) ? std::cerr : std::cout;
        stream << "[" << levelToString(level) << "] " << message << '\n';
    }

} // namespace Nox::detail
