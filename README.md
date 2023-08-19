# C++ 3D Engine

## Technologies

SDL2, SDL2_image, SDL2_ttf, glew, opengl

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
    PerspectiveCamera camera(45.f, renderer.getAspect(), 0.1f, 1000.f);
    Scene3D scene;

    
    // load textures
    Texture::load("stone", "stonebrick_cracked.png");

    // cannot create a cube if shaders aren't loaded
    auto cube = std::make_shared<Cube>(1, "stone");

    // add the entity to the scene
    scene.add(cube);

    // place the camera so it looks the entity
    camera.setPosition(3, 2, 3);
    camera.lookAt(0, 0, 0);

    while(!renderer.shouldClose()) {
        // update ...
        doStuffHere();

        // render
        renderer.render(&scene, &camera);

        // update input
        renderer.updateInput();
    }

    return 0;
}
```

# Demos

Available in the `src/demos/` folder.

## Licence

Read the [MIT licence](./LICENCE).