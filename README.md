# NoxEngine

A C++20 3D engine with a friendly, expressive API.  
Inspired by Three.js idioms — adapted for modern C++.

```cpp
Engine engine({ .title="Hello", .width=1280, .height=720 });
Scene3D scene;
PerspectiveCamera camera(45.f, engine.aspect(), 0.1f, 1000.f);

auto mesh = std::make_shared<Mesh>(
    Geometry::box(1.f, 1.f, 1.f),
    Material::standard()
);
scene.add(mesh);
scene.add(std::make_shared<DirectionalLight>(Color::White, 1.f));
camera.setPosition(3.f, 2.f, 3.f);
camera.lookAt(0.f, 0.f, 0.f);

engine.run([&](float dt) {
    mesh->rotate(0.f, 60.f * dt, 0.f);
    engine.render(scene, camera);
});
```

---

## Requirements

| Tool         | Minimum version           |
| ------------ | ------------------------- |
| CMake        | 3.25                      |
| MSVC         | Visual Studio 2022 (v143) |
| GCC          | 13                        |
| Clang        | 16                        |
| C++ standard | C++20                     |

### Dependencies

Install via [vcpkg](https://vcpkg.io) (recommended) or manually:

| Library    | vcpkg package                              |
| ---------- | ------------------------------------------ |
| SDL3       | `sdl3`                                     |
| GLM        | `glm`                                      |
| stb        | `stb`                                      |
| fastgltf   | `fastgltf`                                 |
| Dear ImGui | `imgui[core,sdl3-binding,opengl3-binding]` |

---

## Building

### Option A — Windows 11, CMake + MSVC (recommended to start)

```bat
git clone https://github.com/yourname/NoxEngine.git
cd NoxEngine

REM Copy and edit the local paths template
copy paths.local.cmake.example paths.local.cmake
notepad paths.local.cmake

cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The sandbox executable will be at `build/sandbox/Release/NoxEngineSandbox.exe`.

### Option B — Windows 11, CMake + Ninja (VSCode / command line)

```bat
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Option C — Windows 11 via WSL2 (Ubuntu 22.04+)

```bash
# Inside WSL terminal
sudo apt update && sudo apt install -y cmake ninja-build gcc-13 g++-13 \
    libgl1-mesa-dev libsdl3-dev

git clone https://github.com/yourname/NoxEngine.git
cd NoxEngine
cp paths.local.cmake.example paths.local.cmake
# Edit paths.local.cmake with Linux/WSL paths

cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_COMPILER=gcc-13 -DCMAKE_CXX_COMPILER=g++-13
cmake --build build
```

### Option D — Linux native (Ubuntu 22.04+ / Debian)

```bash
sudo apt update && sudo apt install -y cmake ninja-build gcc-13 g++-13 \
    libgl1-mesa-dev libsdl3-dev libglm-dev

git clone https://github.com/yourname/NoxEngine.git
cd NoxEngine
cp paths.local.cmake.example paths.local.cmake

cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/sandbox/NoxEngineSandbox
```

### paths.local.cmake

If you manage dependencies manually (not via vcpkg), copy the example file and
fill in the paths matching your machine:

```bash
cp paths.local.cmake.example paths.local.cmake
```

This file is **gitignored** — never commit it.  
See `paths.local.cmake.example` for all available variables and example values
for Windows, WSL, and Linux.

---

## Project structure

```
NoxEngine/
├── engine/                  ← static library (NoxEngine)
│   ├── include/NoxEngine/   ← public headers — the API you consume
│   └── src/                 ← internal implementation
├── sandbox/                 ← example executable
│   └── src/main.cpp         ← usage example
├── cmake/                   ← CMake helpers
├── paths.local.cmake.example
└── CMakeLists.txt
```

---

## Usage (consuming the library)

### CMake integration

After building, link against the `NoxEngine` target:

```cmake
add_subdirectory(NoxEngine)   # or use find_package after install

target_link_libraries(MyApp PRIVATE NoxEngine)
```

### Minimal example

```cpp
#include <NoxEngine/NoxEngine.hpp>
using namespace Nox;

int main() {
    Engine engine({ .title = "My App", .width = 1280, .height = 720 });

    Scene3D scene;
    PerspectiveCamera camera(60.f, engine.aspect(), 0.1f, 500.f);

    // Geometry + material
    auto geo = Geometry::sphere(1.f, 32, 16);
    auto mat = Material::standard();
    mat->setColor(Color(0.2f, 0.6f, 1.f));
    mat->setRoughness(0.3f);

    auto sphere = std::make_shared<Mesh>(geo, mat);
    sphere->setPosition(0.f, 0.f, 0.f);
    scene.add(sphere);

    // Lighting
    auto light = std::make_shared<DirectionalLight>(Color::White, 1.f);
    light->setDirection(-1.f, -2.f, -1.f);
    scene.add(light);
    scene.add(std::make_shared<AmbientLight>(Color::White, 0.05f));

    // Camera
    camera.setPosition(0.f, 2.f, 5.f);
    camera.lookAt(0.f, 0.f, 0.f);

    // Import a model
    // auto model = engine.load("models/character.glb");
    // scene.add(model);

    engine.run([&](float dt) {
        sphere->rotate(0.f, 45.f * dt, 0.f);
        engine.render(scene, camera);
    });
}
```

### Object transforms

```cpp
mesh->setPosition(1.f, 0.f, 0.f);
mesh->setRotation(0.f, 90.f, 0.f);   // degrees, Euler XYZ
mesh->setScale(2.f);                  // uniform
mesh->setScale(1.f, 2.f, 1.f);       // non-uniform
mesh->translate(0.f, 0.1f * dt, 0.f);
mesh->rotate(0.f, 90.f * dt, 0.f);
```

### Built-in geometries

```cpp
Geometry::box(width, height, depth)
Geometry::sphere(radius, widthSegments, heightSegments)
Geometry::plane(width, height)
Geometry::cylinder(radiusTop, radiusBottom, height, radialSegments)
Geometry::fromVertices(vertices, indices)    // custom
```

### Materials

```cpp
// PBR — lit
auto mat = Material::standard();
mat->setColor(Color(r, g, b));
mat->setRoughness(0.5f);
mat->setMetalness(0.0f);
mat->setAlbedoMap("path/to/texture.png");
mat->setNormalMap("path/to/normal.png");

// Unlit — ignores lighting
auto unlit = Material::unlit();
unlit->setColor(Color::White);
unlit->setAlbedoMap("path/to/texture.png");
```

### Lights

```cpp
// Directional — infinite distance, parallel rays (sun)
auto dir = std::make_shared<DirectionalLight>(color, intensity);
dir->setDirection(x, y, z);

// Point — emits in all directions from a position
auto pt = std::make_shared<PointLight>(color, intensity);
pt->setPosition(x, y, z);
pt->setRange(10.f);

// Ambient — uniform global illumination
auto amb = std::make_shared<AmbientLight>(color, intensity);
```

---

## Contributing

Contributions are welcome. Please read the conventions in
`.github/copilot-instructions.md` before opening a PR.

### Code style

- C++20, MSVC/GCC/Clang clean with warnings as errors.
- No `using namespace std`. No raw `new`/`delete`. No global mutable state.
- Private members use trailing underscore (`width_`, `renderQueue_`).
- Public section before private in class definitions.
- Run clang-format before committing (config at `.clang-format`).

### Commit messages

```
feat(renderer): add point light shadow maps
fix(scene):     dirty flag not propagated through deep hierarchy
refactor(math): move AABB into its own header
docs:           update README build instructions for WSL
```

### Pull request checklist

- [ ] Compiles on Windows MSVC with no warnings
- [ ] Compiles on Linux GCC 13 with no warnings  
- [ ] Sandbox example still runs correctly
- [ ] No new raw `new`/`delete` introduced
- [ ] Public API follows the friendly API conventions
- [ ] `paths.local.cmake.example` updated if new path variables were added
