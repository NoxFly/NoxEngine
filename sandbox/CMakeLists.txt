# Copyright (c) 2026 NoxFly — AGPL-3.0

add_executable(NoxEngineSandbox
    src/main.cpp
)

target_link_libraries(NoxEngineSandbox PRIVATE NoxEngine)

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
endif()
