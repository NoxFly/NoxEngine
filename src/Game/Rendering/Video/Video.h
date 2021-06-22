#ifndef VIDEO_H
#define VIDEO_H

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "IniSet.h"
#include "utils.h"

class Video {
	public:
        Video();
		Video(IniSet& config, const std::string& section);
		~Video();

        void clear(Color clearColor);
        void swapWindow();

        void show();
        void hide();
        bool isInitialized() const;
        
        void setMouseGrab(const bool grabbed);
        void setMouseFocus(const bool focus);
        bool isMouseGrabbed() const;
        bool isMouseFocused() const;

    protected:
        bool InitSDL(IniSet& config, const std::string& section);
        bool InitGL(IniSet& config, const std::string& section);
        void destroy();

        SDL_Window* window;
        SDL_GLContext glContext;
        bool isInit;
};

#endif // VIDEO_H