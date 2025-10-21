# Version of CMake
cmake_minimum_required (VERSION 3.12)

# Name of the project (can be changed)
set(PROJECT_NAME "NoxEngine")
set(LIB_NAME "NoxEngine")

project ("${PROJECT_NAME}")

# Load configuration file
include(config.cmake OPTIONAL)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the source files (only .cpp files, headers will be included via target_include_directories)
file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")

# Add the include directory of the project
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


## --- LIBRARIES CONFIGURATION --- ##
# Note: Library paths are defined in config.cmake
# If config.cmake is not found, you can set these variables manually or via environment variables

# Validate that required variables are set
if(NOT GLEW_INCLUDE_DIR)
    message(FATAL_ERROR "GLEW_INCLUDE_DIR is not set. Please configure config.cmake")
endif()

if(NOT SDL_INCLUDE_DIR)
    message(FATAL_ERROR "SDL_INCLUDE_DIR is not set. Please configure config.cmake")
endif()

if(NOT GLM_INCLUDE_DIR)
    message(FATAL_ERROR "GLM_INCLUDE_DIR is not set. Please configure config.cmake")
endif()
## --- --- --- --- --- --- --- --- ##


# Enable Hot-reload the the MSVC compilers if this is supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# Create the shared library
add_library(${LIB_NAME} SHARED ${LIB_SOURCES})

# Windows-specific: Export all symbols automatically
if(WIN32)
    set_target_properties(${LIB_NAME} PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
    )
endif()

# Set the library output directory
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")  # For .dll on Windows
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")  # For .so on Unix
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")  # For .lib on Windows (import library)

# Create the "bin/" directory if it doesn't exist
file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH})

# DLL paths are now defined directly in config.cmake
# No need to deduce them - they're already set!

# Create bin/include/NoxEngine directory for public headers
set(PUBLIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/include")
set(PUBLIC_INCLUDE_SUBDIR "${PUBLIC_INCLUDE_DIR}/NoxEngine")
set(HEADERS_STAMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/headers_copied.stamp")

# Ensure the directory exists for configuration time
file(MAKE_DIRECTORY ${PUBLIC_INCLUDE_DIR})

# Get all header files to track dependencies
file(GLOB_RECURSE HEADER_FILES "${PROJECT_INCLUDE_DIR}/*.hpp" "${PROJECT_INCLUDE_DIR}/*.h" "${PROJECT_INCLUDE_DIR}/*.inl")

# Create a custom command that copies headers and creates a stamp file
add_custom_command(
    OUTPUT ${HEADERS_STAMP_FILE}
    DEPENDS ${HEADER_FILES}
    COMMAND ${CMAKE_COMMAND} -E echo "Copying public headers to bin/include/NoxEngine/"
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PUBLIC_INCLUDE_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PUBLIC_INCLUDE_SUBDIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_INCLUDE_DIR}
            ${PUBLIC_INCLUDE_SUBDIR}
    # Remove .cpp files from public headers using our cleanup script
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/scripts/remove_cpp_from_headers.cmake"
    # Create stamp file to mark completion
    COMMAND ${CMAKE_COMMAND} -E touch ${HEADERS_STAMP_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Installing public headers to bin/include/NoxEngine/"
    VERBATIM
)

# Create a custom target that depends on the stamp file
add_custom_target(CopyHeaders ALL DEPENDS ${HEADERS_STAMP_FILE})


# Copy GLEW DLL based on build configuration
add_custom_command(
    TARGET ${LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<$<CONFIG:Debug>:${GLEW_DLL_DEBUG}>
            $<$<CONFIG:Release>:${GLEW_DLL_RELEASE}>
            ${LIBRARY_OUTPUT_PATH}
    COMMENT "Copying GLEW DLL for $<CONFIG> configuration"
)

# Copy SDL2 DLLs if they exist
if(EXISTS ${SDL2_DLL})
    add_custom_command(
        TARGET ${LIB_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SDL2_DLL}
                ${LIBRARY_OUTPUT_PATH}
        COMMENT "Copying SDL2.dll"
    )
endif()

if(EXISTS ${SDL2_IMAGE_DLL})
    add_custom_command(
        TARGET ${LIB_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SDL2_IMAGE_DLL}
                ${LIBRARY_OUTPUT_PATH}
        COMMENT "Copying SDL2_image.dll"
    )
endif()

if(EXISTS ${SDL2_TTF_DLL})
    add_custom_command(
        TARGET ${LIB_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SDL2_TTF_DLL}
                ${LIBRARY_OUTPUT_PATH}
        COMMENT "Copying SDL2_ttf.dll"
    )
endif()

# Include directories for the library
# PRIVATE: Only for building the library itself, not propagated to consumers
target_include_directories(${LIB_NAME} PRIVATE
    ${PROJECT_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR} 
    ${SDL_INCLUDE_DIR} 
    ${GLM_INCLUDE_DIR}
)

# Define NOXENGINE_EXPORTS when building the library (for __declspec(dllexport))
# Define DEBUG macro for Debug builds
target_compile_definitions(${LIB_NAME} PRIVATE 
    NOXENGINE_EXPORTS
    $<$<CONFIG:Debug>:DEBUG>
)

# Link against the external libraries
# Keep PUBLIC for propagation (examples need these libraries too)
target_link_libraries(${LIB_NAME} PUBLIC
    $<$<CONFIG:Debug>:${GLEW_LIBRARY_DEBUG};${SDL_LIBRARY};OpenGL32.lib>
    $<$<CONFIG:Release>:${GLEW_LIBRARY_RELEASE};${SDL_LIBRARY};OpenGL32.lib>
)

# Option to build examples
option(BUILD_EXAMPLES "Build example executables" ON)

if(BUILD_EXAMPLES)
    # Create examples directory
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/examples")
    
    # Find all example files
    file(GLOB_RECURSE EXAMPLE_FILES "examples/*.cpp")
    
    foreach(EXAMPLE_FILE ${EXAMPLE_FILES})
        # Get the filename without extension
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE} NAME_WE)
        
        # Use just the filename as the target name (no directory prefix)
        # This makes it consistent with tasks.json and launch.json expectations
        set(EXAMPLE_TARGET ${EXAMPLE_NAME})
        
        # Create executable for this example
        add_executable(${EXAMPLE_TARGET} ${EXAMPLE_FILE})
        
        # Examples depend on the library being built first (to generate headers)
        add_dependencies(${EXAMPLE_TARGET} ${LIB_NAME})
        
        # Link against the NoxEngine library (PRIVATE to avoid inheriting build includes)
        target_link_libraries(${EXAMPLE_TARGET} PRIVATE ${LIB_NAME})
        
        # Use ONLY the public headers from bin/include, not src/
        # This simulates using the library as an external package
        target_include_directories(${EXAMPLE_TARGET} PRIVATE 
            ${PUBLIC_INCLUDE_DIR}
            ${GLEW_INCLUDE_DIR}
            ${SDL_INCLUDE_DIR}
            ${GLM_INCLUDE_DIR}
        )
        
        # Set working directory to project root (for resource access)
        set_target_properties(${EXAMPLE_TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/examples"
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        
        # Copy NoxEngine.dll to examples directory after build
        add_custom_command(
            TARGET ${EXAMPLE_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:${LIB_NAME}>
                    "${CMAKE_CURRENT_SOURCE_DIR}/bin/examples"
            COMMENT "Copying NoxEngine.dll to examples directory"
        )
        
        # Copy dependency DLLs to examples directory
        add_custom_command(
            TARGET ${EXAMPLE_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<$<CONFIG:Debug>:${GLEW_DLL_DEBUG}>
                    $<$<CONFIG:Release>:${GLEW_DLL_RELEASE}>
                    "${CMAKE_CURRENT_SOURCE_DIR}/bin/examples"
            COMMENT "Copying GLEW DLL to examples directory"
        )
        
        # Copy SDL2 DLLs to examples directory
        if(EXISTS ${SDL2_DLL})
            add_custom_command(
                TARGET ${EXAMPLE_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${SDL2_DLL}
                        "${CMAKE_CURRENT_SOURCE_DIR}/bin/examples"
                COMMENT "Copying SDL2.dll to examples directory"
            )
        endif()
        
        if(EXISTS ${SDL2_IMAGE_DLL})
            add_custom_command(
                TARGET ${EXAMPLE_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${SDL2_IMAGE_DLL}
                        "${CMAKE_CURRENT_SOURCE_DIR}/bin/examples"
                COMMENT "Copying SDL2_image.dll to examples directory"
            )
        endif()
        
        if(EXISTS ${SDL2_TTF_DLL})
            add_custom_command(
                TARGET ${EXAMPLE_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${SDL2_TTF_DLL}
                        "${CMAKE_CURRENT_SOURCE_DIR}/bin/examples"
                COMMENT "Copying SDL2_ttf.dll to examples directory"
            )
        endif()
    endforeach()
endif()

# Visual Studio specific: Set the property to use with MSBuild (set to library)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "${LIB_NAME}")
