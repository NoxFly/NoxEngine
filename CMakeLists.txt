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

# Set the library output directory
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")

# Create the "bin/" directory if it doesn't exist
file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH})

# DLL paths are now defined directly in config.cmake
# No need to deduce them - they're already set!

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
target_include_directories(${LIB_NAME} PUBLIC 
    $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
    ${GLEW_INCLUDE_DIR} 
    ${SDL_INCLUDE_DIR} 
    ${GLM_INCLUDE_DIR}
)

# Link against the external libraries
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
        
        # Get the relative directory path
        file(RELATIVE_PATH EXAMPLE_REL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/examples ${EXAMPLE_FILE})
        get_filename_component(EXAMPLE_DIR ${EXAMPLE_REL_PATH} DIRECTORY)
        
        # Create a unique target name
        if(EXAMPLE_DIR)
            string(REPLACE "/" "_" EXAMPLE_TARGET "${EXAMPLE_DIR}_${EXAMPLE_NAME}")
        else()
            set(EXAMPLE_TARGET ${EXAMPLE_NAME})
        endif()
        
        # Create executable for this example
        add_executable(${EXAMPLE_TARGET} ${EXAMPLE_FILE})
        
        # Link against the NoxEngine library
        target_link_libraries(${EXAMPLE_TARGET} PRIVATE ${LIB_NAME})
        
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
