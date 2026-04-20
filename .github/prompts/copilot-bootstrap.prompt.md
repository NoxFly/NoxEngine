---
agent: 'agent'
model: Claude Opus 4.6 (copilot)
name: foundations
description: This prompt is used to bootstrap the initial file structure and source code for NoxEngine.
---

# Copilot Bootstrap Prompt — NoxEngine Initial Architecture

## PROMPT

You are bootstrapping the initial file structure and source code for **NoxEngine**,
a C++20 3D engine static library with a separate sandbox executable.
Follow the `.github/copilot-instructions.md` and the `README.md` file in this repository strictly for
all conventions, architecture decisions, forbidden patterns, and library choices.

Update the `roadmap.md` file after your development.

---

### What to generate

Generate **all files listed below**, complete and compilable. Do not use placeholders
or TODO stubs for the files marked ★. Files marked ○ can be minimal but must be
syntactically valid and correctly wired into CMake.

---

### File list

#### Build system

★ `CMakeLists.txt` (root)
  - CMake 3.25+ minimum
  - C++20 required
  - Includes `paths.local.cmake` with OPTIONAL (no error if absent)
  - Adds subdirectories: engine, sandbox
  - Sets output directories for binaries and libs

★ `paths.local.cmake.example`
  - Documents every path variable the build may need
  - Variables: SDL3_DIR, GLM_DIR, STB_INCLUDE_DIR, FASTGLTF_DIR, IMGUI_DIR
  - Each variable has a comment explaining what it points to and an example Windows path
  - Also shows how to set these for Linux/macOS

○ `.gitignore`
  - Ignores: build/, cmake-build-*/, .vs/, *.user, paths.local.cmake,
    *.lib, *.a, *.dll, *.so, *.exe (except sandbox output), .cache/

★ `engine/CMakeLists.txt`
  - Defines target NoxEngine as STATIC library
  - Globs src/ for .cpp files
  - PUBLIC include: engine/include/
  - PRIVATE include: engine/src/
  - Links: SDL3, OpenGL, GLM (header only), stb_image (header only), fastgltf, ImGui
  - Enables all warnings, warnings-as-errors, per platform (MSVC /W4 /WX, GCC/Clang -Wall -Wextra -Wpedantic -Werror)
  - Defines NOX_OPENGL for the OpenGL backend

★ `sandbox/CMakeLists.txt`
  - Defines target NoxEngineSandbox as executable
  - Links NoxEngine (which transitively brings all includes)
  - Sets working directory to sandbox/ for assets

---

#### Engine public headers (engine/include/NoxEngine/)

★ `NoxEngine.hpp`  — facade, includes all public subsystem headers

★ `core/Logger.hpp`
  - Enum class LogLevel { Trace, Info, Warn, Error, Fatal }
  - Free functions: NOX_LOG_INFO(...), NOX_LOG_WARN(...), NOX_LOG_ERROR(...)
  - Implemented with std::format
  - Thread-safe (std::mutex on the output)

★ `core/Assert.hpp`
  - NOX_ASSERT(expr) macro: in Debug fires with file/line message, in Release is a no-op
  - NOX_ASSERT_MSG(expr, msg) variant with a custom message

★ `core/HandlePool.hpp`
  - Template class HandlePool<T>
  - Inner struct Handle { uint32_t index; uint32_t generation; bool valid() const; }
  - Methods: Handle insert(T), T* get(Handle), void remove(Handle)
  - Free list for slot reuse, generation counter per slot

★ `core/Signal.hpp`
  - Template class Signal<Args...>
  - Methods: uint32_t connect(std::function<void(Args...)>), void disconnect(uint32_t), void emit(Args...)
  - Stores slots in std::vector<pair<uint32_t, std::function<...>>>

★ `math/Types.hpp`
  - using Vec2 = glm::vec2; Vec3, Vec4, Mat3, Mat4, Quat
  - using AABB = struct with glm::vec3 min, max — plus bool contains(Vec3), bool intersects(AABB)
  - All in namespace Nox::Math

★ `renderer/RHI.hpp`
  - Handle types: BufferHandle, TextureHandle, ShaderHandle, PipelineHandle (all wrap uint32_t + generation)
  - Descriptor structs: BufferDesc, TextureDesc, ShaderDesc, PipelineDesc (use designated-initializer-friendly layout)
  - Abstract class RHI with pure virtual: createBuffer, createTexture, createShader, createPipeline,
    destroyBuffer, destroyTexture, destroyShader, destroyPipeline,
    beginFrame() -> CommandBuffer*, endFrame(CommandBuffer*)
  - Abstract class CommandBuffer with pure virtual: setViewport, bindPipeline,
    bindVertexBuffer, bindIndexBuffer, pushConstant, drawIndexed, drawIndexedInstanced

★ `renderer/Renderer.hpp`
  - Class Renderer owns a unique_ptr<RHI>
  - Methods: submit(RenderCommand), render(Camera&), onResize(int w, int h)
  - Internally holds std::vector<RenderCommand> renderQueue_ (pre-reserved, cleared each frame)
  - RenderCommand struct: PipelineHandle, BufferHandle vbo, BufferHandle ibo,
    uint32_t indexCount, glm::mat4 worldMatrix, float depth

★ `scene/Transform.hpp`
  - Class Transform: position (Vec3), rotation (Quat), scale (Vec3)
  - Methods: localMatrix() const -> Mat4, computeWorld(const Mat4& parentWorld) -> Mat4
  - Dirty flag — recompute only when needed

★ `scene/SceneNode.hpp`
  - Class SceneNode: name, Transform, vector of child unique_ptr<SceneNode>, weak_ptr to parent
  - Template methods: addComponent<T>(args...), getComponent<T>() -> T*
  - void update(float dt, const Mat4& parentWorld = identity)

★ `platform/Window.hpp`
  - Class Window wrapping SDL3
  - Constructor(title, width, height)
  - Methods: pollEvents() -> bool (returns false on quit), swapBuffers(), size() -> {w,h}
  - Signal<int,int> onResize
  - Signal<> onClose

---

#### Engine implementation (engine/src/)

★ `core/Logger.cpp`       — implements Logger (std::format + std::mutex)
★ `renderer/OpenGLRHI.hpp` + `OpenGLRHI.cpp`
   - Concrete RHI using OpenGL 4.6 DSA
   - createBuffer  → glCreateBuffers + glNamedBufferStorage
   - createTexture → glCreateTextures + glTextureStorage2D
   - Manages internal maps: unordered_map<uint32_t, GLuint> for each resource type
★ `renderer/Renderer.cpp`
★ `scene/Transform.cpp`
★ `scene/SceneNode.cpp`
★ `platform/Window.cpp`   — SDL3 window + OpenGL context creation (4.6 core profile)

---

#### Sandbox (sandbox/src/)

★ `main.cpp`
  - Creates a Window (1280×720, "NoxEngine Sandbox")
  - Creates Renderer with OpenGLRHI backend
  - Creates a SceneNode "cube" with a Transform
  - Main loop: pollEvents → update Transform rotation (spin on Y axis) → submit to Renderer → render
  - Logs FPS every second using Logger
  - Clean shutdown in correct order (renderer before window)

---

### Constraints & reminders

- Every public header starts with `#pragma once`.
- No `using namespace std;` anywhere.
- No raw `new` / `delete` — use make_unique / make_shared / HandlePool.
- No `#include <windows.h>` in any public header.
- All OpenGL calls use DSA only (glCreate*, glNamed*, glTextureStorage*).
- CMake links are target-based only (`target_link_libraries`, `target_include_directories`).
- `paths.local.cmake` is included with OPTIONAL in root CMakeLists.txt.
- The sandbox working directory must be set so relative asset paths work from sandbox/.
- All structs used as descriptors (PipelineDesc etc.) must have default member values
  so designated initializers work with partial initialization.
- Use `std::string_view` for read-only string parameters, not `const std::string&`.
- Private data members use trailing underscore (e.g. `width_`, `renderQueue_`).
- Public section comes before private in class definitions.

### Current development environment

This project is currently being developed on **Windows 11** using **CMake** with
**Visual Studio 2022** or **VSCode + CMake Tools extension**.
Generated code and CMake scripts must compile and run correctly on:

- **Windows 11 — CMake + MSVC 2022** (primary development target right now)
- **Windows 11 — WSL2 (Ubuntu 22.04+) — CMake + GCC or Clang**
- **Linux (Ubuntu 22.04+ / Debian) — CMake + GCC 13+ or Clang 16+**

Cross-platform rules:
- Never use `\` as path separator in code — always use `std::filesystem::path` or `/`.
- No `#include <windows.h>` in any public or private header unless inside
  `#ifdef _WIN32` guards, and only in `.cpp` files.
- Platform-specific code lives in `engine/src/platform/` behind a compile-time
  abstraction (`#ifdef _WIN32 / __linux__`).
- CMake `find_package` calls must work on all three targets.
  `paths.local.cmake` provides hints via `CMAKE_PREFIX_PATH` or `*_DIR` variables.
- Line endings: repository uses LF (`.gitattributes` enforces this).
- The `paths.local.cmake.example` must show Windows paths, WSL paths,
  and Linux native paths in comments for every variable.

You have to create the .vscode/ directory with appropriate `tasks.json` and `launch.json` for building and debugging on Windows and Linux with VSCode.

### Friendly public API

The engine must expose a **Three.js-inspired high-level API** as described in
`.github/copilot-instructions.md` under "User-Facing API Design".
All generated rendering-facing code must go through this high-level API.
The low-level RHI layer (OpenGLRHI, CommandBuffer, HandlePool) is **internal only**
and must never appear in sandbox code or user-facing examples.


### Specific to this computer

The location of the cpp libraries on this computer is : C:/perso/cpp-libs/<libname-with-version>/

- SDL3: C:/perso/cpp-libs/SDL3-3.4.4
- SDL3_image: C:/perso/cpp-libs/SDL3_image-3.4.2
- GLM: C:/perso/cpp-libs/glm-1.0.3/
- GLEW: C:/perso/cpp-libs/glew-2.2.0/ (2.1.0 available)
- FMT: C:/perso/cpp-libs/fmt-12.1.0/ (I don't know if this is needed, but in case of, it's here)

A documentation is available in the github repos of SDL3 / SDL3_image.
You'll maybe need to build the libraries from source to get the .lib files for SDL3.
Documentation might be present inside the given folders, or you can find it online.

On Linux, we include the libraries by their name then the path we need. For instance, for SDL3, we would do `#include <SDL3/SDL.h>`.

On Windows, we should take care of that and configure so we include the same manner on both platforms.

DO NOT use SDL2.
