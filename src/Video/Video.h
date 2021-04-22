#ifndef VIDEO_H
#define VIDEO_H

#include <string>
#include <SDL2/SDL.h>

class Video {
	public:
        Video();
		Video(unsigned int x, unsigned int y, unsigned int width, unsigned int height, std::string title, Uint32 flags=0);
		~Video();

        SDL_Window* createWindow(unsigned int x, unsigned int y, unsigned int width, unsigned int height, std::string title, Uint32 flags=SDL_WINDOW_SHOWN);
        Uint32 id() const;
        void close();
        bool hasWindow() const;

    protected:
        SDL_Window* m_window;
		SDL_GLContext m_oglContext;

        int initSDL();
        int initGL();
        void destroyWindow(bool destroyContext);
};

#endif // VIDEO_H