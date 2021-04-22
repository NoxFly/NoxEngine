#ifndef CONFIG_H
#define CONFIG_H

#include <string>

typedef struct {
    int windowWidth = 1280;
    int windowHeight = 720;
    bool isFullscreen = false;
    int renderDistance = 16;
    int fov = 90;
    std::string appName = "Engine | SDL & OpenGL";
    int opengl_major_version = 1;
    int opengl_minor_version = 4;
} Config;

Config getConfigFromFile(std::string filename);

#endif // CONFIG_H