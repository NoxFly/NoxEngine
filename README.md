# C++ 3D Engine

SDL2, SDL2_image, SDL2_ttf, glew, opengl


## Install dependencies

### Linux
```sh
# sdl2
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
# glew, glm and opengl utils
sudo apt-get install libglm-dev libglew-dev glew-utils mesa-utils
```

### Windows

1. Using [MingW64 (MSYS2)](https://www.msys2.org/).
    1. Ensure you're up-to-date with MingW64 (`pacman -Suy` and `pacman -Syuu`)
    1. Ensure you have `C:/msys64/usr/bin` in environment variables.
    1. Ensure you have gcc, make and rsync installed
        ```
        pacman -S gcc rsync mingw-w64-i686-gcc mingw-w64-x86_64-gcc mingw-w64-x86_64-make
        ```
    1. With an administrator command prompt, paste these instructions to be able to run `make` :
        * Create a symlink of make : `mklink C:\msys64\mingw64\bin\make C:\msys64\mingw64\bin\mingw32-make.exe`


## Usage

The goal is to use this engine as static or shared library.

For now it's only usable as shared one.

## Shared library
```sh
./run.sh --shared -v
```

This will generate a :
* `bin/lib/NoxEngine.so/dll` depending of your OS.
* and an include folder in `bin/lib/include/NoxEngine`.

Copy the `.so` to your project executable's folder, and move the `/bin/lib/include/NoxEngine` folder to :
* `/usr/local/include/NoxEngine` folder on Linux.
* any folder that you will link through your IDE on Windows.

## How to use the engine

There're some basic examples in the `examples/` folder.

to run these :

```sh
cd examples
make 2D # to run 2D engine
make 3D # same for 3D one
```

### Minimal working example
```ini
# this file is optional, the engine can load without it.
[ENGINE]
title=NoxEngine window
width=1280
height=720
fps=50
OPENGL_MAJOR_VERSION=4
OPENGL_MINOR_VERSION=6
fullscreen=0
background=153,204,255
icon=favicon.png
multisampling_buffer=1
multisampling_amples=16

[PATH]
resources=/resources/
images=/resources/images/
shaders=/resources/shaders/
textures=/resources/textures/

# ... some other sections that could be useful in your project
```
```cpp

// this loads all necessary things for the needed dimension
// in engine.hpp
// this also declare either the macro _2D or _3D.
// 2D
// #define __NOX_ENGINE_2D__
// 3D
#define __NOX_ENGINE_3D__

#include <core/engine.hpp>
#include <glm/glm.hpp>

using namespace NoxEngine;


int main() {
    IniSet config;

    if(!config.loadFromFile("./config.ini")) {
        Console::error("Failed to load the configuration.\nExiting.");
        return 1;
    }

    Renderer renderer(config);

    const auto fov = glm::radians(config.getFloatValue("CAMERA", "fov"));
    const auto near = config.getFloatValue("CAMERA", "near");
    const auto far = config.getFloatValue("CAMERA", "far");

    PerspectiveCamera camera(fov, renderer.getAspect(), near, far);
    Scene3D scene;

    // load shaders
    Shader::setShadersPath(config.getValue("PATH", "shaders"));
    Shader::setDefaultGLSLversion(renderer.getCompactGLversion());
    Shader::loadFolder();

    // comment the texture loading if you don't have
    // load textures
    Texture::setTexturesPath(config.getValue("PATH", "textures"));
    Texture::load("stone", "stonebrick_cracked.png");

    // cannot create a cube if shaders aren't loaded
    auto cube = std::make_shared<Cube>(1, "stone");

    // add the entity to the scene
    scene.add(cube);

    // place the camera so it looks the entity
    camera.setPosition(3, 2, 3);
    camera.lookAt(0, 0, 0);

    while(!renderer.shouldClose()) {
        renderer.updateInput();
        // do stuff ...
        renderer.render(&scene, &camera);
    }

    return 0;
}
```

## Licence

Read the [MIT licence](./LICENCE).