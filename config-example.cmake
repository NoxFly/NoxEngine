# =============================================================================
# NoxEngine - External Libraries Configuration (EXAMPLE)
# =============================================================================
# Copy this file to config.cmake and update the paths to match your system
# 
# Structure:
# - Each library defines a ROOT directory
# - Include and library paths are defined relative to the root
# - DLL paths are explicitly defined (relative to root)
# =============================================================================

# -----------------------------------------------------------------------------
# GLEW - OpenGL Extension Wrangler Library
# -----------------------------------------------------------------------------
set(GLEW_ROOT "C:/path/to/glew-2.2.0")

set(GLEW_INCLUDE_DIR "${GLEW_ROOT}/include")

set(GLEW_LIBRARY_DEBUG "${GLEW_ROOT}/lib/Debug/x64/glew32d.lib")
set(GLEW_LIBRARY_RELEASE "${GLEW_ROOT}/lib/Release/x64/glew32.lib")

# DLL paths (relative to root)
set(GLEW_DLL_DEBUG "${GLEW_ROOT}/bin/Debug/x64/glew32d.dll")
set(GLEW_DLL_RELEASE "${GLEW_ROOT}/bin/Release/x64/glew32.dll")


# -----------------------------------------------------------------------------
# SDL2 - Simple DirectMedia Layer (with extensions)
# -----------------------------------------------------------------------------
set(SDL2_ROOT "C:/path/to/SDL2-2.26.2")

set(SDL_INCLUDE_DIR "${SDL2_ROOT}/include")

# SDL2 libraries as a list (easier to maintain)
# Add or remove libraries as needed
set(SDL_LIBRARY 
    "${SDL2_ROOT}/VisualC/x64/Debug/SDL2.lib"
    "${SDL2_ROOT}/VisualC/x64/Debug/SDL2main.lib"
    "${SDL2_ROOT}/VisualC/x64/Debug/SDL2_image.lib"
    "${SDL2_ROOT}/VisualC/x64/Debug/SDL2_ttf.lib"
)

# DLL paths
# For pre-built SDL2, DLLs are typically in bin/ directory
set(SDL2_DLL "${SDL2_ROOT}/bin/SDL2.dll")
set(SDL2_IMAGE_DLL "${SDL2_ROOT}/bin/SDL2_image.dll")
set(SDL2_TTF_DLL "${SDL2_ROOT}/bin/SDL2_ttf.dll")


# -----------------------------------------------------------------------------
# GLM - OpenGL Mathematics
# -----------------------------------------------------------------------------
set(GLM_ROOT "C:/path/to/glm-0.9.9")

set(GLM_INCLUDE_DIR "${GLM_ROOT}/include")
# GLM is header-only, no library or DLL needed
