# Copyright (c) 2026 NoxFly — AGPL-3.0

# ── Source files ────────────────────────────────────────────────
file(GLOB_RECURSE NOX_ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ── NoxEngine static library ───────────────────────────────────
add_library(NoxEngine STATIC ${NOX_ENGINE_SOURCES})

# ── Ensure consistent MSVC runtime (static /MT) ────────────────────────────
# if(MSVC)
#     set_target_properties(NoxEngine PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
# endif()

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
    set(fastgltf_DIR "${FASTGLTF_DIR}" CACHE PATH "" FORCE)
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
    target_include_directories(NoxEngine PUBLIC "${IMGUI_DIR}" "${IMGUI_DIR}/backends")
    target_compile_definitions(NoxEngine PUBLIC NOX_HAS_IMGUI)
endif()

# ── Jolt Physics (optional) ────────────────────────────────────
if(JOLT_DIR)
    # Forcer AVANT l'inclusion — même config Debug/Release que le projet parent
    set(USE_STATIC_MSVC_RUNTIME_LIBRARY ON  CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS               OFF CACHE BOOL "" FORCE)
    set(CPP_RTTI_ENABLED                ON  CACHE BOOL "" FORCE)
    set(CPP_EXCEPTIONS_ENABLED          ON  CACHE BOOL "" FORCE)

    # Désactiver les tests et outils Jolt — on veut juste la lib
    set(TARGET_UNIT_TESTS   OFF CACHE BOOL "" FORCE)
    set(TARGET_HELLO_WORLD  OFF CACHE BOOL "" FORCE)
    set(TARGET_PERFORMANCE_TEST OFF CACHE BOOL "" FORCE)
    set(TARGET_SAMPLES      OFF CACHE BOOL "" FORCE)
    set(TARGET_VIEWER       OFF CACHE BOOL "" FORCE)

    add_subdirectory("${JOLT_DIR}/Build" jolt_build)
    target_link_libraries(NoxEngine PRIVATE Jolt)
    target_compile_definitions(NoxEngine PRIVATE NOX_HAS_JOLT)
else()
    message(STATUS "NoxEngine: Jolt not found — physics disabled")
endif()

# ── SDL3_mixer (optional) ──────────────────────────────────────
if(SDL3_MIXER_DIR)
    target_include_directories(NoxEngine PUBLIC "${SDL3_MIXER_DIR}/include")
    if(MSVC)
        target_link_libraries(NoxEngine PUBLIC
            "$<IF:$<CONFIG:Debug>,${SDL3_MIXER_DIR}/lib/SDL3_mixer-static.lib,${SDL3_MIXER_DIR}/lib/SDL3_mixer-static.lib>"
        )
    else()
        target_link_libraries(NoxEngine PUBLIC "${SDL3_MIXER_DIR}/lib/libSDL3_mixer-static.a")
    endif()
    target_compile_definitions(NoxEngine PUBLIC NOX_HAS_SDL3_MIXER)
else()
    message(STATUS "NoxEngine: SDL3_mixer not found — audio disabled")
endif()

# ── SDL3_net (optional) ────────────────────────────────────────
if(SDL3_NET_DIR)
    target_include_directories(NoxEngine PUBLIC "${SDL3_NET_DIR}/include")
    if(MSVC)
        target_link_libraries(NoxEngine PUBLIC
            "$<IF:$<CONFIG:Debug>,${SDL3_NET_DIR}/lib/SDL3_net-static.lib,${SDL3_NET_DIR}/lib/SDL3_net-static.lib>"
        )
        # SDL3_net uses Winsock2 on Windows
        target_link_libraries(NoxEngine PUBLIC ws2_32 iphlpapi)
    else()
        target_link_libraries(NoxEngine PUBLIC "${SDL3_NET_DIR}/lib/libSDL3_net-static.a")
    endif()
    target_compile_definitions(NoxEngine PUBLIC NOX_HAS_SDL3_NET)
else()
    message(STATUS "NoxEngine: SDL3_net not found — networking disabled")
endif()

# ── Lua (optional) ─────────────────────────────────────────────
if(LUA_DIR)
    target_include_directories(NoxEngine PUBLIC "${LUA_DIR}/include")
    if(MSVC)
        target_link_libraries(NoxEngine PUBLIC
            "$<IF:$<CONFIG:Debug>,${LUA_DIR}/lib/lua_static.lib,${LUA_DIR}/lib/lua_static.lib>"
        )
    else()
        target_link_libraries(NoxEngine PUBLIC "${LUA_DIR}/lib/liblua_static.a")
    endif()
    target_compile_definitions(NoxEngine PUBLIC NOX_HAS_LUA)
else()
    message(STATUS "NoxEngine: Lua not found — scripting disabled")
endif()

# ── sol2 (header only, optional) ───────────────────────────────
if(SOL2_DIR)
    target_include_directories(NoxEngine SYSTEM PUBLIC "${SOL2_DIR}/include")
    target_compile_definitions(NoxEngine PUBLIC NOX_HAS_SOL2)
endif()
