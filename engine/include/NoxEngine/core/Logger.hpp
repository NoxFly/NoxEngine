// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <cstdint>
#include <format>
#include <mutex>
#include <string_view>

namespace Nox {

    enum class LogLevel : uint8_t {
        Trace,
        Info,
        Warn,
        Error,
        Fatal
    };

    namespace detail {

        void logMessage(LogLevel level, std::string_view message);

        template<typename... Args>
        void logFormatted(LogLevel level, std::format_string<Args...> fmt, Args&&... args) {
            logMessage(level, std::format(fmt, std::forward<Args>(args)...));
        }

    } // namespace detail

} // namespace Nox

#define NOX_LOG_TRACE(...) ::Nox::detail::logFormatted(::Nox::LogLevel::Trace, __VA_ARGS__)
#define NOX_LOG_INFO(...)  ::Nox::detail::logFormatted(::Nox::LogLevel::Info,  __VA_ARGS__)
#define NOX_LOG_WARN(...)  ::Nox::detail::logFormatted(::Nox::LogLevel::Warn,  __VA_ARGS__)
#define NOX_LOG_ERROR(...) ::Nox::detail::logFormatted(::Nox::LogLevel::Error, __VA_ARGS__)
#define NOX_LOG_FATAL(...) ::Nox::detail::logFormatted(::Nox::LogLevel::Fatal, __VA_ARGS__)
