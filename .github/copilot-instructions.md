# NoxEngine - AI Coding Agent Instructions

## Project Overview
NoxEngine is a C++23 3D rendering engine using OpenGL, SDL2 (with SDL2_image and SDL2_ttf), GLEW, and GLM. Originally Unix-focused, now supports Windows with CMake.

## Architecture & Structure

### Core Components
- **Namespace**: All code lives in `NoxEngine` namespace
- **Entry Point**: `engine.hpp` is the main include - it defines load order (preload/postload sections)
- **Scene Graph**: `Scene` contains `Actor` objects (meshes, lights, cameras)
- **Rendering Pipeline**: `Renderer` → `Scene` → `Camera` → `Actor::render()`
- **Resource Management**: Static `ResourceHolder<T, Key>` pattern for shaders/textures (see `Shader::m_bank`)

### Key Directories
```
src/core/Actor/        # Renderable objects, geometries, materials, shaders, textures
src/core/Camera/       # PerspectiveCamera, OrthographicCamera
src/core/Controls/     # Camera controls (PointerLock, Orbit, Trackball, Map)
src/core/Renderer/     # Main renderer, Input handling
src/core/Scene/        # Scene graph management
src/IniSet/            # Config file parser (res/config/config.ini)
src/Console/           # Logging utilities
examples/              # Working demos (base3D.cpp, camera/pointerLock.cpp)
```

## Build System

The project uses the CMake build system to support cross-platform builds on Windows (MSVC/MinGW) and Unix-like systems.
The CMake configuration builds a **shared library** (`NoxEngine.dll` on Windows, `NoxEngine.so` on Unix) from all source files in the `src/` directory, outputting to the `bin/` directory.

Examples in the `examples/` folder are built as separate executables that link against the NoxEngine shared library, and are placed in `bin/examples/`.

### Library Names (MinGW64 vs Unix)
- **GLEW**: `glew32` (Windows) vs `GLEW` (Unix)
- **OpenGL**: `opengl32` (Windows) vs `OpenGL` (Unix)
- **SDL2**: Same name on both platforms (SDL2, SDL2_image, SDL2_ttf)

### VS Code Tasks
- `.vscode/tasks.json`: CMake configure/build tasks using Visual Studio tools or MinGW
- `.vscode/launch.json`: Debugger config (cppvsdbg for MSVC, gdb for MinGW)
- `.vscode/c_cpp_properties.json`: IntelliSense paths for MSYS2 includes


## Code Patterns & Conventions

### Resource Loading
- **Shaders**: Auto-loaded from `res/assets/shaders/` via `Shader::loadFolder()` in `Renderer` constructor
  - Supports `#include <chunks/file.glsl>` preprocessor directive (see `Shader::readAndPrecomputeFile`)
  - Named by filename without extension: `basic3D.vert` → `Shader::get("basic3D")`
- **Textures**: `Texture::load("name", "filename.png")` then `Texture::get("name")`
- **Models**: `Mesh3D::loadFromFile("cube.obj")` from `res/assets/models/`

### Actor Hierarchy
```cpp
Actor (base)
├── Light → AmbientLight
├── Camera → PerspectiveCamera, OrthographicCamera
└── Object3D → Mesh3D, Cube, Sphere
```

### Material System
- `BasicMaterial`: Simple color/texture rendering (uses `basic3D` shader)
- `PhongMaterial`: Phong lighting (uses `lightPhong3D` shader) - **Note**: Currently has a bug (shapes render black)
- Materials reference shaders via `Shader::get("shaderName")`

### Configuration
- `res/config/config.ini`: Engine settings (window size, OpenGL version, paths)
- Loaded via `IniSet` class: `config.getValue("SECTION", "key")`
- Paths are relative to working directory (must run from project root)

### Common Gotchas
1. **uint typedef**: Already defined in `engine.typedef.hpp` - don't redefine
2. **GLM_ENABLE_EXPERIMENTAL**: Required for some GLM features (already in `engine.typedef.hpp`)
3. **main() signature**: Use `int main(int argc, char** argv)` for SDL2 compatibility
4. **Windows subsystem**: Do NOT use `-mwindows` flag; keep console subsystem for debugging
5. **Working directory**: Examples expect to run from project root (where `res/` exists)

## Development Workflow

### Adding New Features
1. **New Actor Type**: Inherit from `Actor` or `Object3D`, implement in `src/core/Actor/Objects/`
2. **New Material**: Inherit from `Material`, create corresponding shader in `res/assets/shaders/`
3. **New Control**: Inherit from `CameraControl`, implement in `src/core/Controls/3D/`

### Debugging
- Use VS Code launch config (`(Windows) Launch`) for debugging
- Check `Console::error()` calls for runtime issues
- Renderer initializes GLEW - check `Renderer::Renderer()` for OpenGL context setup

### Testing Changes
- Modify or create example in `examples/` directory
- Build and run via VS Code tasks or CMake directly

## External Dependencies
- **SDL2** (2.x): Window management, input, image loading (SDL2_image), fonts (SDL2_ttf)
- **GLEW**: OpenGL extension loading
- **GLM**: Math library (header-only, requires `GLM_ENABLE_EXPERIMENTAL`)
- **OpenGL**: 4.6 core profile (configurable in config.ini)

## License
AGPL-3.0 - see `LICENCE` file
