#include "Video_driver.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <GL/glew.h>
#include <array>
#include <sstream>

#include "Console.h"

Video Video_driver::video;

int Video_driver::opengl_major_version = 1;
int Video_driver::opengl_minor_version = 1;
bool Video_driver::is_initialized = false;

bool Video_driver::loadLastGlVersion() {
    Video_driver::video.createWindow(0, 0, 0, 0, "dummy", VIDEO_HIDDEN);
    
    if(!Video_driver::video.hasWindow()) {
        Console::error("Video_driver::loadLastGlVersion", "Cannot load openGL version");
        return false;
    }

    std::string GL_version;

    try {
        GL_version = std::string((char*)glGetString(GL_VERSION));
    } catch(std::exception const &e) {
        Console::error("Video_driver::loadLastGlVersion", "Cannot recover GL_VERSION :\n" + (std::string)e.what());
        Video_driver::video.close();
        return false;
    }

    std::array<int, 2> v = { -1, -1 };

    int i = -1,
        j = 0;

    for(char const c : GL_version) {
        if(isdigit(c)) {
            v[j++] = c - '0';

            if(j == 2)
                break;
        }
    }

    Video_driver::video.close();

    if(v[0] > 0 && v[1] > 0) {
        Video_driver::opengl_major_version = v[0];
        Video_driver::opengl_minor_version = v[1];
        Video_driver::is_initialized = true;

        std::ostringstream oss;
        oss << "Load OpenGL " << v[0] << "." << v[1];
        Console::info("Video_driver::loadLastGlVersion", oss.str());

        return true;
    }

    return false;
}