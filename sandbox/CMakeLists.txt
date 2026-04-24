# Copyright (c) 2026 NoxFly — AGPL-3.0

file(GLOB_RECURSE SANDBOX_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(NoxEngineSandbox ${SANDBOX_SOURCES})

target_link_libraries(NoxEngineSandbox PRIVATE NoxEngine)

target_include_directories(NoxEngineSandbox PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# ── MSVC Runtime: suppress LNK4098 — SDL3 DLL uses /MD while we use /MT ─────
if(MSVC)
    target_link_options(NoxEngineSandbox PRIVATE
        $<$<CONFIG:Debug>:/NODEFAULTLIB:MSVCRTD>
        $<$<NOT:$<CONFIG:Debug>>:/NODEFAULTLIB:MSVCRT>
    )
endif()

# ── Compiler warnings ──────────────────────────────────────────
if(MSVC)
    target_compile_options(NoxEngineSandbox PRIVATE /W4 /WX)
else()
    target_compile_options(NoxEngineSandbox PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# ── Working directory set to sandbox/ for asset paths ──────────
set_target_properties(NoxEngineSandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# ── Copy SDL3 DLL next to executable on Windows ────────────────
if(WIN32)
    # Try the imported target first, fall back to finding it relative to SDL3_DIR
    if(TARGET SDL3::SDL3-shared)
        add_custom_command(TARGET NoxEngineSandbox POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL3::SDL3-shared>
                $<TARGET_FILE_DIR:NoxEngineSandbox>
            COMMENT "Copying SDL3.dll to output directory"
        )
    elseif(SDL3_DIR)
        find_file(SDL3_DLL_PATH
            NAMES SDL3.dll
            HINTS "${SDL3_DIR}/../bin" "${SDL3_DIR}/../../bin"
            NO_DEFAULT_PATH
        )
        if(SDL3_DLL_PATH)
            add_custom_command(TARGET NoxEngineSandbox POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${SDL3_DLL_PATH}"
                    $<TARGET_FILE_DIR:NoxEngineSandbox>
                COMMENT "Copying SDL3.dll to output directory"
            )
        endif()
    endif()

    # Copy NoxEngine DLL next to executable
    add_custom_command(TARGET NoxEngineSandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:NoxEngine>
            $<TARGET_FILE_DIR:NoxEngineSandbox>
        COMMENT "Copying NoxEngine.dll to output directory"
    )
endif()
