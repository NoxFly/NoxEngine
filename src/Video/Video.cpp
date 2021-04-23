#include "Video.h"

#include "Video_driver.h"
#include <iostream>
#include <GL/glew.h>

Video::Video():
    m_window(nullptr)
{

}

Video::Video(unsigned int x, unsigned int y, unsigned int width, unsigned int height, std::string title, Uint32 flags):
    m_window(nullptr)
{
    createWindow(x, y, width, height, title, flags);
}

Video::Video(Window_props& properties, Uint32 flags):
    Video(properties.x, properties.y, properties.width, properties.height, properties.title, flags)
{
    
}

Video::~Video() {
    destroyWindow(true);
}

SDL_Window* Video::createWindow(unsigned int x, unsigned int y, unsigned int width, unsigned int height, std::string title, Uint32 flags) {
    if(m_window != nullptr) {
        std::cerr << "[Warning] Video::createWindow : Window already created" << std::endl;
        return nullptr;
    }

    if(initSDL() < 0) {
        return nullptr;
    }

    flags |= SDL_WINDOW_OPENGL;

    m_window = SDL_CreateWindow(
        title.c_str(),
        (int)x,     (int)y,
        (int)width, (int)height,
        flags
    );

    if(m_window == nullptr) {
        std::cerr << "[Error] Video::createWindow : SDL Window Creation (" << SDL_GetError() << ")" << std::endl;
		SDL_Quit();
		return nullptr;
    }

    if(initGL() < 0) {
        return nullptr;
    }

    // disabled : can cause some troubles for some people
    //SDL_SetWindowInputFocus(m_window);

    return m_window;
}

int Video::initSDL() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "[Error] Video::initSDL : SDL Init (" << SDL_GetError() << ")" << std::endl;
		SDL_Quit();
		return -1;
	}

    // OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, Video_driver::opengl_major_version);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, Video_driver::opengl_minor_version);

    // Double Buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    return 0;
}

int Video::initGL() {
    m_oglContext = SDL_GL_CreateContext(m_window);

	if(m_oglContext == nullptr) {
		std::cerr << "[Error] Video::initGL : OpenGL Context Creation (" << SDL_GetError() << ")" << std::endl;
		destroyWindow(false);
		return -3;
	}

    GLenum glew_init = glewInit();

    if(glew_init != GLEW_OK) {
        std::cerr << "[Error] Engine::initGL : Glew Init (" << glewGetErrorString(glew_init) << ")" << std::endl;
        destroyWindow(true);
        return -4;
    }

	// Depth Buffer activation
	glEnable(GL_DEPTH_TEST);

    return 0;
}

void Video::close() {
    destroyWindow(true);
}

void Video::destroyWindow(bool destroyContext=false) {
    if(destroyContext && m_oglContext != nullptr) {
        SDL_GL_DeleteContext(m_oglContext);
    }

    if(m_window != nullptr) {
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }
}


Uint32 Video::id() const {
    return SDL_GetWindowID(m_window);
}

bool Video::hasWindow() const {
    return m_window != nullptr;
}

Window_props Video::getWindowProps() const {
    if(!hasWindow()) {
        return (Window_props){ 0, 0, 0, 0, "" };
    }

    int x, y, w, h;

    SDL_GetWindowSize(m_window, &w, &h);
    SDL_GetWindowPosition(m_window, &x, &y);
    const char* title = SDL_GetWindowTitle(m_window);

    return (Window_props){ x, y, w, h, title };
}