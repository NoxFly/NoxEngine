# Copyright (c) 2026 NoxFly — AGPL-3.0

# ── Source files ────────────────────────────────────────────────
file(GLOB_RECURSE NOX_ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ── NoxEngine static library ───────────────────────────────────
add_library(NoxEngine STATIC ${NOX_ENGINE_SOURCES})

target_include_directories(NoxEngine
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# ── C++20 ───────────────────────────────────────────────────────
target_compile_features(NoxEngine PUBLIC cxx_std_20)

# ── Compiler warnings ──────────────────────────────────────────
if(MSVC)
    target_compile_options(NoxEngine PRIVATE /W4 /WX)
else()
    target_compile_options(NoxEngine PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# ── Backend define ──────────────────────────────────────────────
target_compile_definitions(NoxEngine PRIVATE NOX_OPENGL)

# ── SDL3 ────────────────────────────────────────────────────────
find_package(SDL3 REQUIRED CONFIG)
target_link_libraries(NoxEngine PUBLIC SDL3::SDL3)

# ── OpenGL ──────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)
target_link_libraries(NoxEngine PUBLIC OpenGL::GL)

# ── GLEW ────────────────────────────────────────────────────────
if(GLEW_DIR)
    target_include_directories(NoxEngine PUBLIC "${GLEW_DIR}/include")
    if(MSVC)
        target_link_libraries(NoxEngine PUBLIC
            "$<IF:$<CONFIG:Debug>,${GLEW_DIR}/lib/Debug/x64/glew32sd.lib,${GLEW_DIR}/lib/Release/x64/glew32s.lib>"
        )
    else()
        target_link_libraries(NoxEngine PUBLIC "${GLEW_DIR}/lib/libGLEW.a")
    endif()
    target_compile_definitions(NoxEngine PUBLIC GLEW_STATIC)
else()
    find_package(GLEW REQUIRED)
    target_link_libraries(NoxEngine PUBLIC GLEW::GLEW)
endif()

# ── GLM (header only) ──────────────────────────────────────────
if(GLM_DIR)
    target_include_directories(NoxEngine PUBLIC "${GLM_DIR}")
else()
    find_package(glm REQUIRED CONFIG)
    target_link_libraries(NoxEngine PUBLIC glm::glm)
endif()

# ── stb_image (header only, optional for now) ──────────────────
if(STB_INCLUDE_DIR)
    target_include_directories(NoxEngine PRIVATE "${STB_INCLUDE_DIR}")
    target_compile_definitions(NoxEngine PRIVATE NOX_HAS_STB_IMAGE)
endif()

# ── fastgltf (optional for now) ────────────────────────────────
if(FASTGLTF_DIR)
    find_package(fastgltf REQUIRED CONFIG)
    target_link_libraries(NoxEngine PRIVATE fastgltf::fastgltf)
    target_compile_definitions(NoxEngine PRIVATE NOX_HAS_FASTGLTF)
endif()

# ── Dear ImGui (optional for now) ──────────────────────────────
if(IMGUI_DIR)
    file(GLOB IMGUI_SOURCES
        "${IMGUI_DIR}/imgui.cpp"
        "${IMGUI_DIR}/imgui_draw.cpp"
        "${IMGUI_DIR}/imgui_tables.cpp"
        "${IMGUI_DIR}/imgui_widgets.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    )
    target_sources(NoxEngine PRIVATE ${IMGUI_SOURCES})
    target_include_directories(NoxEngine PRIVATE "${IMGUI_DIR}" "${IMGUI_DIR}/backends")
    target_compile_definitions(NoxEngine PRIVATE NOX_HAS_IMGUI)
endif()
