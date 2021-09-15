# C++ 3D Engine

SDL2, SDL2_image, SDL2_ttf, glew

tested on Windows 10 with Mingw64.

build project :
```
./run.sh -v
```

Add the `-f` option to remove the object file if you encounter compilation's errors.

The executable is in the `out/` folder.

I've intentionally added the SDL2, Glew and GLM libraries in the project's folder, so you can directly compile the project.

# How to use the engine

```cpp
#include "IniSet.h" // to load .ini config
#include "Renderer.h" // manages video and input with SDL2
#include "Scene.h" // stocks entities informations that have to be rendered
#include "PerspectiveCamera.h" // Inherits from Camera Base class

int main() {
    IniSet config;
    
    if(!config.loadFromFile('./path/to/config.ini')) {
        std::cout << "Unable to load config" << std::endl;
        return 1;
    }

    Renderer renderer(config);
    Scene scene();
    PerspectiveCamera camera(75.0, renderer.getAspect(), 0.1, 100.0);


    while(!renderer.shouldClose()) {
        renderer.updateInput();

        // do stuff

        renderer.render(scene, camera);
    }

    return 0;
}
```

## Licence

Read the [MIT licence](./LICENCE).