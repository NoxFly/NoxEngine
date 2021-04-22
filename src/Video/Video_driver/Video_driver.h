#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include "Video.h"

class Video_driver {
	public:
        static bool loadLastGlVersion();

        static int opengl_major_version;
        static int opengl_minor_version;

    private:
        static Video video;
};

#endif // VIDEO_DRIVER_H