/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef ENGINE_EXPORT_HPP
#define ENGINE_EXPORT_HPP

// Export/Import macros for Windows DLL
#ifdef _WIN32
    #ifdef NOXENGINE_EXPORTS
        #define NOXENGINE_API __declspec(dllexport)
    #else
        #define NOXENGINE_API __declspec(dllimport)
    #endif
#else
    // On Unix-like systems, symbols are exported by default
    #define NOXENGINE_API
#endif

#endif // ENGINE_EXPORT_HPP
