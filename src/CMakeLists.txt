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

file(GLOB_RECURSE NOXENGINE_SRC
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB_RECURSE NOXENGINE_HEADERS
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/include/*.hpp
    ${CMAKE_SOURCE_DIR}/include/*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.inl
)

target_sources(NoxEngine
    PRIVATE ${NOXENGINE_SRC}
    PUBLIC  ${NOXENGINE_HEADERS}
)

# =========================
# Include directories
# =========================

get_target_property(SDL2_INCLUDE_DIR SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)

target_include_directories(${NOXENGINE_TARGET}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${SDL2_INCLUDE_DIR}
        $<BUILD_INTERFACE:${SDL2_INCLUDE_DIR}/SDL2>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# =========================
# Compile definitions
# =========================

target_compile_definitions(${NOXENGINE_TARGET}
    PRIVATE
        NOXENGINE_INTERNAL
        GLM_ENABLE_EXPERIMENTAL
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
        OpenGL::GL
        glm::glm
    PRIVATE
        GLEW::GLEW
        Threads::Threads
        SDL2::SDL2
        SDL2::SDL2main
        SDL2::SDL2_image
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
