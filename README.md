# C++ 3D Engine

## Technologies

SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, glew, opengl

Working with the [nfpm](https://github.com/NoxFly/nfpm) tool to compile, run and manage project.

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
resources=res/
images=res/images/
shaders=res/shaders/
textures=res/textures/

# ... some other sections that could be useful in your project
```
```cpp
#include <iostream>

#define __NOX_ENGINE_3D__

#include "core/engine.hpp"
#include "Console.hpp"
#include "IniSet.hpp"

using namespace NoxEngine;

int main(int argc, char** argv) {
	IniSet config;

	if (!config.loadFromFile("config/config.ini")) {
		Console::error("main", "Failed to load configuration");
		return EXIT_FAILURE;
	}

	Renderer renderer(config);
	Scene3D scene;
	PerspectiveCamera camera(45.f, renderer.getAspect(), 0.1f, 1000.f);

	Texture::load("stone", "stonebrick_cracked.png");

	auto cube = std::make_shared<Cube>(1.f, "stone", Color(150, 50, 10));
	cube->setPosition(0.f, 0.f, 0.f);

	scene.add(cube);


	auto light = std::make_shared<AmbientLight>(Color(255, 255, 255), 50.f);
	light->setPosition(5.f, 2.f, 2.f);

	scene.add(light);


	camera.setPosition(3.f, 2.f, 3.f);
	camera.lookAt(0.f, 0.f, 0.f);

	while (!renderer.shouldClose()) {
		renderer.render(scene, camera);

		cube->rotate(0.1f, 0.f, 0.05f);

		renderer.updateInput();
	}


	return EXIT_SUCCESS;
}
```

# Demos

Available in the `src/examples/` folder.

```sh
nf run -v --shared # build the engine
nf example base3D # run the example with built engine as shared library
```

## Licence

Read the [AGPL licence](./LICENCE).
