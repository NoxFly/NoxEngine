# Copyright (c) 2026 NoxFly — AGPL-3.0

cmake_minimum_required(VERSION 3.25)
project(NoxEngine VERSION 0.1.0 LANGUAGES CXX)

# ── C++20 required ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── MSVC Runtime: static CRT (/MTd or /MT) — matches pre-built third-party libs ──
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ── Output directories ─────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ── Local paths (gitignored, user-specific) ─────────────────────
include(${CMAKE_SOURCE_DIR}/paths.local.cmake OPTIONAL)

# ── Subdirectories ──────────────────────────────────────────────
add_subdirectory(engine)
add_subdirectory(sandbox)
