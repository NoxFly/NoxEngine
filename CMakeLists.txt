# Copyrights 2026 NoxFly

cmake_minimum_required(VERSION 3.15)

project(
    NoxEngine
    VERSION 1.0.0
    LANGUAGES CXX
)

# =========================
# Global configuration
# =========================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# =========================
# Options
# =========================

option(NOXENGINE_BUILD_SHARED "Build NoxEngine as shared library" ON)
option(NOXENGINE_BUILD_EXAMPLES "Build engine examples" OFF)
option(NOXENGINE_ENABLE_ASSERTS "Enable engine assertions" ON)

# =========================
# Dependencies
# =========================

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

# =========================
# Engine target
# =========================

add_subdirectory(src)

# =========================
# Examples (optional)
# =========================

if (NOXENGINE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()