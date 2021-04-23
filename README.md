# OpenGL & SDL2 3D Engine

## Prerequires

  /  |  /
-----|-----
c++  |  17
bash |  >= 5.0
SDL  |  2
GLEW |  >= 2.1

The engine uses multithreading.

## Usage

```
./run.sh [options]
```

Options can be `-d` to debug with gdb, `-r` for release, `-f` to rebuild, `-v` for verbosing the compilation.

## Installing dependencies

* Glew : `sudo apt-get install libglew-dev`
* SDL : `sudo apt-get install libsdl2-dev libsdl2-image-dev`
* Check the supported OpenGL version you have : `glxinfo | grep "OpenGL version"`