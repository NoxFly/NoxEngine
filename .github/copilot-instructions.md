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
- The engine compiles as a **static library** (`NoxEngine.lib` / `libNoxEngine.a` / `NoxEngine.dll`).
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

Indent what's inside a namespace.

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

| Library          | Version  | Purpose                                                                     |
| ---------------- | -------- | --------------------------------------------------------------------------- |
| **SDL3**         | 3.x      | Window, input, event loop, audio                                            |
| **GLM**          | 1.x      | Math (Vec, Mat, Quat) — header only                                         |
| **OpenGL**       | 4.6 Core | Rendering backend (DSA API only)                                            |
| **stb_image**    | latest   | Texture loading — header only. The raw .h file is copied into this project. |
| **fastgltf**     | 0.9.0    | glTF 2.0 mesh/scene loading                                                 |
| **Dear ImGui**   | 1.92.x   | Debug UI, editor overlays                                                   |
| **Jolt Physics** | 5.5.0    | Physics (future integration)                                                |

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
- Always add copyrights and license headers to new files (NoxFly 2026 AGPL-3.0).

---

## User-Facing API Design — "Friendly C++ API"

NoxEngine exposes a **high-level, expressive API** inspired by Three.js idioms,
adapted to C++ idioms and ownership semantics. The guiding principle:
**a developer should be able to render a lit 3D scene in under 20 lines of code.**

### Design goals

- Sensible defaults everywhere — nothing requires configuration unless you want to customize.
- Method chaining where it reads naturally (builder-style setters return `*this`).
- No raw pointers in the public API. Factory methods return `std::shared_ptr<T>`.
  (Shared ownership is correct here: a mesh can belong to multiple scene nodes;
  a material can be shared across many meshes.)
- Geometry, Material, and Mesh are separate, composable objects — matching the
  Three.js mental model while staying C++ idiomatic.
- `scene.add(object)` is the universal entry point for anything renderable or influential.
- Importing a full model file is one line. Manual geometry construction is also one line.

### Reference API shape (enforce this in all generated code)

```cpp
// --- Engine + window bootstrap ---
EngineConfig config;
config.title  = "My App";
config.width  = 1280;
config.height = 720;
config.vsync  = true;
Engine engine(config); // owns Window + Renderer + RHI

Scene3D scene;

PerspectiveCamera camera(45.0f, engine.aspect(), 0.1f, 1000.0f);

// --- Geometry (CPU-side shape description) ---
auto box     = Geometry::box(1.0f, 1.0f, 1.0f);
auto sphere  = Geometry::sphere(0.5f, 32, 16);
auto plane   = Geometry::plane(10.0f, 10.0f);
auto custom  = Geometry::fromVertices(vertices, indices);

// --- Material ---
auto mat = Material::standard();

mat->setColor(Color(1.0f, 0.5f, 0.2f));
mat->setRoughness(0.4f);
mat->setAlbedoMap("textures/stone.png");

auto unlit = Material::unlit();
unlit->setColor(Color::White);

// --- Mesh = Geometry + Material ---
auto mesh = std::make_shared<Mesh>(box, mat);
mesh->setPosition(0.0f, 0.5f, 0.0f);
mesh->setRotation(0.0f, 45.0f, 0.0f);   // degrees, Euler XYZ
mesh->setScale(1.0f);
scene.add(mesh);

// --- Import a full model (glTF) ---
auto model = engine.load("models/character.glb");   // returns SceneNode subtree
scene.add(model);

// --- Lights ---
auto sun = std::make_shared<DirectionalLight>(Color::White, 1.0f);
sun->setDirection(-1.0f, -1.0f, -0.5f);
scene.add(sun);

auto point = std::make_shared<PointLight>(Color(1.0f, 0.8f, 0.6f), 2.0f);
point->setPosition(2.0f, 3.0f, 2.0f);
point->setRange(10.0f);
scene.add(point);

auto ambient = std::make_shared<AmbientLight>(Color::White, 0.05f);
scene.add(ambient);

// --- Camera placement ---
camera.setPosition(3.0f, 2.0f, 3.0f);
camera.lookAt(0.0f, 0.0f, 0.0f);

// --- Main loop ---
engine.run([&](float dt) {
    mesh->rotate(0.0f, 90.0f * dt, 0.0f);   // rotate Y, degrees/sec
    engine.render(scene, camera);
});
```

### Rules derived from the above

- `Geometry` is a **value-producing factory** (static methods only, no public constructor).
- `Material` is a **factory + fluent setter** object. `Material::standard()` returns
  `std::shared_ptr<Material>`. All setters return `void` (no chaining needed on materials).
- `Mesh`, `DirectionalLight`, `PointLight`, `AmbientLight` all inherit from `SceneObject`.
  `scene.add()` accepts `std::shared_ptr<SceneObject>`.
- `Object3D` (base of Mesh and lights) exposes:
  `setPosition(x,y,z)`, `setRotation(x,y,z)` (degrees Euler),
  `setScale(uniform)` / `setScale(x,y,z)`,
  `rotate(x,y,z)`, `translate(x,y,z)`,
  `getTransform() -> const Transform&`.
- `engine.run(loopFn)` drives the main loop. `loopFn` receives `float dt` in seconds.
  The loop handles events, calls the user function, renders, and swaps buffers.
- `engine.load(path)` returns `std::shared_ptr<SceneNode>` with the full node hierarchy
  from the file. The developer adds it to the scene directly.
- Angles in the public API are always **degrees** (converted to radians internally).
- Colors in the public API are always **linear float RGB(A)** in range [0, 1].
