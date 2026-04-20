// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <cstdlib>
#include <iostream>

#ifdef NDEBUG
    #define NOX_ASSERT(expr) ((void)0)
    #define NOX_ASSERT_MSG(expr, msg) ((void)0)
#else
    #define NOX_ASSERT(expr) \
        do { \
            if (!(expr)) { \
                std::cerr << "Assertion failed: " #expr \
                          << "\n  File: " << __FILE__ \
                          << "\n  Line: " << __LINE__ << '\n'; \
                std::abort(); \
            } \
        } while (false)

    #define NOX_ASSERT_MSG(expr, msg) \
        do { \
            if (!(expr)) { \
                std::cerr << "Assertion failed: " #expr \
                          << "\n  Message: " << (msg) \
                          << "\n  File: " << __FILE__ \
                          << "\n  Line: " << __LINE__ << '\n'; \
                std::abort(); \
            } \
        } while (false)
#endif
