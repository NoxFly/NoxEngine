# Copyrights 2026 NoxFly

set(NOXENGINE_TARGET NoxEngine)

if (NOXENGINE_BUILD_SHARED)
    add_library(${NOXENGINE_TARGET} SHARED)
else()
    add_library(${NOXENGINE_TARGET} STATIC)
endif()

add_library(Nox::Engine ALIAS ${NOXENGINE_TARGET})

# =========================
# Sources
# =========================

target_sources(${NOXENGINE_TARGET}
    PRIVATE
        internal/Window.cpp
        internal/Renderer.cpp
        internal/ShaderCompiler.cpp
        internal/AssetManager.cpp
        internal/Scene.cpp

    PUBLIC
        include/NoxEngine/Engine.hpp
        include/NoxEngine/Window.hpp
        include/NoxEngine/Renderer.hpp
        include/NoxEngine/Scene.hpp
)

# =========================
# Include directories
# =========================

target_include_directories(${NOXENGINE_TARGET}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/internal
)

# =========================
# Compile definitions
# =========================

target_compile_definitions(${NOXENGINE_TARGET}
    PRIVATE
        NOXENGINE_INTERNAL
    PUBLIC
        $<$<BOOL:${NOXENGINE_ENABLE_ASSERTS}>:NOXENGINE_ENABLE_ASSERTS>
)

# =========================
# Compiler warnings
# =========================

if (MSVC)
    target_compile_options(${NOXENGINE_TARGET} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${NOXENGINE_TARGET} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# =========================
# Link libraries
# =========================

target_link_libraries(${NOXENGINE_TARGET}
    PUBLIC
        SDL2::SDL2
        OpenGL::GL
    PRIVATE
        GLEW::GLEW
        Threads::Threads
)

# =========================
# Output directories
# =========================

set_target_properties(${NOXENGINE_TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    POSITION_INDEPENDENT_CODE ON
)
