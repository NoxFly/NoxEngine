// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

/// @brief DLL export/import macro for NoxEngine shared library.
/// When building the engine (NOX_ENGINE_EXPORTS defined by CMake), symbols are exported.
/// When consuming the engine, symbols are imported.
#if defined(_WIN32) || defined(_WIN64)
    #ifdef NOX_ENGINE_EXPORTS
        #define NOX_API __declspec(dllexport)
    #else
        #define NOX_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef NOX_ENGINE_EXPORTS
        #define NOX_API __attribute__((visibility("default")))
    #else
        #define NOX_API
    #endif
#else
    #define NOX_API
#endif
