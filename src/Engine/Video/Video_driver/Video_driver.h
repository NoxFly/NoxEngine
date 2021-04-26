#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include "Video.h"


enum {
    VIDEO_POS_CENTER            = SDL_WINDOWPOS_CENTERED,
    VIDEO_POS_UNDEFINED         = SDL_WINDOWPOS_UNDEFINED,
    VIDEO_ALWAYS_ON_TOP         = SDL_WINDOW_ALWAYS_ON_TOP,
    VIDEO_BORDERLESS            = SDL_WINDOW_BORDERLESS,
    VIDEO_FOREIGN               = SDL_WINDOW_FOREIGN,
    VIDEO_FULLSCREEN            = SDL_WINDOW_FULLSCREEN,
    VIDEO_FULLSCREEN_DESKTOP    = SDL_WINDOW_FULLSCREEN_DESKTOP,
    VIDEO_HIDDEN                = SDL_WINDOW_HIDDEN,
    VIDEO_MAXIMIZED             = SDL_WINDOW_MAXIMIZED,
    VIDEO_MINIMIZED             = SDL_WINDOW_MINIMIZED,
    VIDEO_MOUSE_CAPTURE         = SDL_WINDOW_MOUSE_CAPTURE,
    VIDEO_OPENGL                = SDL_WINDOW_OPENGL
};



class Video_driver {
	public:
        static bool loadLastGlVersion();

        static int opengl_major_version;
        static int opengl_minor_version;

        static bool is_initialized;

    private:
        static Video video;
};

#endif // VIDEO_DRIVER_H