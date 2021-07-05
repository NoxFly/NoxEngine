# Minecraft in C++

SDL2, SDL2_image, SDL2_ttf, glew

tested on Windows 10 with Mingw64.

build project :
```
./run.sh -v
```

Add the `-f` option to remove the object file if you encounter compilation's errors.

The executable is in the `out/` folder.

I've intentionally added the SDL2, Glew and GLM libraries in the project's folder, so you can directly compile the project.

## Next steps

1. [ ] Create a player class so this entity moves and the camera moves depending on it. Transfert the input's managment about the camera's displacement.
1. [ ] Do the chunk's memory management when the camera moves.
1. [ ] Create a folder where each block is registered in a file with its properties and ID, so we can use TileData.blockType.
1. [ ] Create a test folder, add some unit tests, update the Makefile consequently.
1. [ ] Look for memory leaks and performances issues.
1. [ ] Improve the Drawable parts to support multi-textures on an object.
1. [ ] Improve shaders to support lights and shadows.
1. [ ] Implement 2D GUI.

## Licence

Read the [MIT licence](./LICENCE).