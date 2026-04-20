# NoxEngine — Copilot Instructions

## Project Overview

NoxEngine is a C++ 3D engine built from scratch, structured as a **static library** consumed
by a separate sandbox executable. The goal is a clean, modern, architecturally sound codebase
that can later support multiple rendering backends (OpenGL → Vulkan → DirectX).

---

## Language & Standard

- **C++20** strictly. No C++17 fallbacks, no C++23 features (compiler support uneven).
- Compile with warnings as errors: `-Wall -Wextra -Wpedantic` (GCC/Clang), `/W4 /WX` (MSVC).
- Use `[[nodiscard]]`, `[[likely]]`, `[[unlikely]]`, `[[maybe_unused]]` where appropriate.
- Prefer `std::format` over `printf` / `std::ostringstream`.
- Use `std::span` instead of `(T* ptr, size_t count)` pairs.
- Use designated initializers for config/descriptor structs.
- Use `if constexpr` for compile-time branching in templates.
- Use concepts to constrain templates — never raw `enable_if`.

---

## Toolchain & Build

- **CMake 3.25+** with modern target-based configuration (no global `include_directories`).
- **vcpkg** for third-party dependencies (manifest mode, `vcpkg.json`).
- Compiler support: MSVC 2022, GCC 13+, Clang 16+.
- Build types: `Debug`, `Release`, `RelWithDebInfo`.
- The engine compiles as a **static library** (`NoxEngine.lib` / `libNoxEngine.a`).
- The sandbox compiles as an **executable** that links against the engine lib.
- Local dependency paths live in `paths.local.cmake` (gitignored). A template
  `paths.local.cmake.example` is always committed.

---

## Repository Structure

```
NoxEngine/
├── .github/
│   └── copilot-instructions.md
├── engine/
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── NoxEngine/           ← public API headers only
│   │       ├── NoxEngine.hpp    ← single include facade
│   │       ├── core/
│   │       ├── renderer/
│   │       ├── scene/
│   │       └── math/
│   └── src/                     ← implementation (.cpp) — not visible to consumers
│       ├── core/
│       ├── renderer/
│       ├── scene/
│       └── math/
├── sandbox/
│   ├── CMakeLists.txt
│   └── src/
│       └── main.cpp
├── cmake/
│   └── FindPackages.cmake
├── CMakeLists.txt
├── paths.local.cmake.example
└── paths.local.cmake            ← gitignored, user-specific paths
```

### Header placement rules

- `engine/include/NoxEngine/` — **public headers only**. Everything a consumer of the lib needs.
- `engine/src/` — implementation + private headers. Never included from public headers directly.
- Forward-declare in public headers whenever possible; include full definitions only in `.cpp`.

---

## Architecture

### Layered dependencies (strict — no upward includes)

```
Application / Sandbox
        ↓
   Scene Graph          (SceneNode, Transform, ECS World)
        ↓
   Renderer             (RHI abstraction, RenderQueue, Camera)
        ↓
   Platform             (Window, Input, Timer — via SDL3)
        ↓
   Math                 (Vec2/3/4, Mat3/4, Quat, AABB, Ray — via GLM)
        ↓
   Core                 (Logger, Assert, HandlePool, Signal, ResourceManager)
```

A layer may only depend on layers **below** it. Never include upward.

### RHI (Rendering Hardware Interface)

- Abstract base class `RHI` with pure virtual methods for resource creation and frame commands.
- Concrete implementations: `OpenGLRHI`, `VulkanRHI` (future), `DirectXRHI` (future).
- Resources identified by **handles** (`BufferHandle`, `TextureHandle`, etc.) — opaque `uint32_t`
  wrappers with a generation counter. Never expose raw GPU resource pointers in the public API.
- Backend selected at engine startup via factory function. No runtime switching.
- `CommandBuffer` abstraction for recording draw calls — never call GL/VK/DX directly from scene code.

### Scene

- **SceneNode** owns a `Transform` and a flat map of components.
- **Transform** caches local and world matrices with dirty flags.
- **ECS World** (`World`) stores components in per-type `ComponentStore<T>` (SoA-friendly).
- Systems are **free functions** or stateless structs that iterate over the World — not methods
  on components.

### Resource Management

- Central `ResourceManager` owns all GPU resources via `HandlePool<T>`.
- Assets loaded asynchronously (C++20 coroutines or `std::future`).
- No two systems hold owning pointers to the same GPU resource.

---

## Coding Conventions

### Naming

```cpp
// Types (classes, structs, enums, concepts, using aliases)
class  SceneNode   {}
struct Vertex      {}
enum class ShaderStage { Vertex, Fragment, Compute }
concept Arithmetic = ...
using EntityId = uint32_t;

// Functions and methods — camelCase
void   uploadMesh(std::span<const Vertex> verts);
float  deltaTime() const;

// Variables — camelCase
int    vertexCount = 0;
float  deltaTime   = 0.0f;

// Private member variables — camelCase with trailing underscore
class Camera {
    float fov_;
    glm::vec3 position_;
};

// Constants and enumerators — PascalCase
constexpr int MaxLights = 8;
enum class CullMode { None, Front, Back };

// Macros (avoid; if unavoidable) — SCREAMING_SNAKE_CASE
#define NOX_ASSERT(expr) ...
```

### File naming

- Headers: `PascalCase.hpp`
- Sources: `PascalCase.cpp`
- One primary class per file, filename matches class name.

### Includes order (clang-format enforced)

```cpp
// 1. Corresponding header (in .cpp)
#include "Renderer.hpp"

// 2. Engine public headers
#include <NoxEngine/core/Logger.hpp>
#include <NoxEngine/math/Vec3.hpp>

// 3. Third-party
#include <glm/glm.hpp>
#include <SDL3/SDL.h>

// 4. Standard library
#include <vector>
#include <memory>
#include <string>
```

### Class layout order

```cpp
class MyClass {
public:
    // 1. Public type aliases / nested types
    // 2. Constructors, destructor
    // 3. Public methods
    // 4. Public static methods / factories

protected:
    // 5. Protected methods only (avoid protected data)

private:
    // 6. Private methods
    // 7. Private data members (trailing underscore)
};
```

---

## Memory & Ownership Rules

- **No raw `new` / `delete`** anywhere in the codebase. Use `std::make_unique` / `std::make_shared`.
- Default to `std::unique_ptr` — it means "I own this, exclusively."
- Use `std::shared_ptr` only when shared ownership is genuinely required (not "for safety").
- **Non-owning observation**: use `const T&` (cannot be null), `const T*` (can be null).
  Never accept `std::shared_ptr<T>` by value just to observe.
- GPU resource handles (`BufferHandle`, `TextureHandle`) are value types. Copy freely.
- Classes that manage a GPU or OS resource **must** follow the **Rule of Five**:
  define or `= delete` copy constructor, copy assignment, move constructor, move assignment,
  and destructor.
- Prefer stack allocation for small, fixed-size, short-lived data.
- `std::vector` for dynamic collections — always `reserve()` when max size is known.
- `std::array<T, N>` for fixed-size collections — zero overhead vs raw array, safer API.

---

## Error Handling

- `assert(condition)` — for **programmer bugs**: precondition violations, impossible states.
  Disabled in Release (`NDEBUG`). Use `NOX_ASSERT` macro for engine-specific messaging.
- `throw std::runtime_error` (or derived) — for **recoverable runtime errors**:
  file not found, device lost, out of GPU memory.
- Never throw from destructors.
- Never use exceptions for control flow (no throw-as-return-value).
- Functions that can fail but are in hot paths should return `std::expected<T, Error>` (C++23)
  or an explicit `Result<T>` type rather than throwing.

---

## Performance Rules

### Hot path (code executed every frame, for every visible object)

- **No heap allocation** — reuse pre-allocated buffers (`renderQueue_.clear()` not reassign).
- **No virtual dispatch** — use templates, concepts, or data-oriented design.
- **No `std::function`** in tight loops — use function pointers or templated callbacks.
- **Cache-friendly data layout** — prefer SoA (Structure of Arrays) over AoS for bulk data.
- **Minimize branching** — sort data by type before processing; avoid per-object type switches.
- Keep per-frame allocations to zero after the warm-up frame.

### General

- Mark non-modifying methods `const`.
- Pass objects by `const T&` when not taking ownership and size > pointer size.
- Use `[[nodiscard]]` on all factory functions and functions returning error codes.
- Never sort or search an `std::list` — use `std::vector`.
- Prefer `std::string_view` over `const std::string&` for read-only string parameters.

---

## What Is Forbidden

```
❌ #include <windows.h> in any public header
❌ using namespace std; (anywhere — use explicit std::)
❌ Raw owning pointers (T* p = new T)
❌ delete / delete[]
❌ std::shared_ptr as a default instead of std::unique_ptr
❌ Virtual methods in hot-path per-object update loops
❌ Global mutable state / Singletons
❌ Inheritance depth > 2 (prefer composition + ECS)
❌ Protected data members
❌ Hardcoded file paths or platform-specific path separators (use std::filesystem::path)
❌ Magic numbers without named constants
❌ C-style casts — use static_cast / reinterpret_cast / const_cast explicitly
❌ std::endl (use '\n' — endl flushes, which is almost never wanted)
❌ Recursive includes — forward-declare in headers, include in .cpp
❌ Thread-unsafe global initialization
```

---

## Third-Party Libraries

| Library | Version | Purpose |
|---|---|---|
| **SDL3** | 3.x | Window, input, event loop, audio |
| **GLM** | 1.x | Math (Vec, Mat, Quat) — header only |
| **OpenGL** | 4.6 Core | Rendering backend (DSA API only) |
| **stb_image** | latest | Texture loading — header only |
| **fastgltf** | latest | glTF 2.0 mesh/scene loading |
| **Dear ImGui** | docking branch | Debug UI, editor overlays |
| **Jolt Physics** | latest | Physics (future integration) |

### OpenGL usage rules

- Use **DSA (Direct State Access)** exclusively — `glCreate*`, `glNamed*`, `glTextureStorage*`.
- Never use the old bind-to-modify pattern (`glBind* / glTex* / glBind* 0`).
- Use `glTextureStorage2D` (immutable storage) over `glTexImage2D`.
- All shader uniform locations cached on pipeline creation — no `glGetUniformLocation` per frame.

---

## CMake Conventions

- Use `target_*` commands exclusively — never `include_directories`, `link_libraries` globally.
- Set `CMAKE_CXX_STANDARD 20` and `CMAKE_CXX_STANDARD_REQUIRED ON` at root.
- Separate `NoxEngine` target (STATIC library) from `NoxEngineSandbox` target (executable).
- Export engine include dirs via `target_include_directories(NoxEngine PUBLIC include/)`.
- All compiler warnings enabled per-target, not globally.
- `paths.local.cmake` included with `include(... OPTIONAL)` — missing file is not a build error
  (useful for CI where vcpkg handles everything).

---

## Git & File Hygiene

- `paths.local.cmake` — **gitignored**. Contains local library paths specific to the developer's machine.
- `paths.local.cmake.example` — **committed**. Template showing all expected variables with comments.
- Generated files (`build/`, `*.user`, `.vs/`, `cmake-build-*/`) — gitignored.
- One logical change per commit. Commit messages: `type(scope): description`
  (e.g., `feat(renderer): add DSA buffer upload`, `fix(scene): dirty flag not propagated`).
