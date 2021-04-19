#include "Engine.h"

#include "Config.h"

#include <signal.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <array>
#include <GL/glew.h>

Engine* Engine::instance = new Engine();

Engine* Engine::getInstance() {
    return Engine::instance;
}

Engine::Engine():
    m_config{},
    m_input(),
    m_paths{},
    m_window(0),
    m_oglContext(0),
    m_firstInitialization(true),
    m_running(true),
    m_stopOnWindowClose(false),
    m_thread_loop(&Engine::loop, this)
{
    m_paths["resources"] = "./";
    m_paths["config"] = "./";

    terminateOnWindowClose();

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = Engine::SIGINT_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}

Engine::~Engine() {
    m_thread_loop.join();
}

void Engine::setPath(std::string name, std::string path) {
    if(m_paths.count(name)) {
        m_paths[name] = path + (path[path.size()-1] != '/'? "/" : "");
    }
}

void Engine::loadConfigFromFile(std::string filename) {
    m_config = getConfigFromFile(m_paths["config"] + filename);
}

bool Engine::loadContext() {
    if(!m_firstInitialization) {
        std::cout << "Engine::loadContext : OpenGL context already has been initialized" << std::endl;
        return true;
    }

    if(initSDL() != 0) {
        std::cout << "Failed to initialize SDL2" << std::endl;
        return false;
    }

    if(initGL() != 0) {
        std::cout << "Failed to initialize OpenGL Context" << std::endl;
        return false;
    }
    
    // check max opengl version
    std::array<int, 2> glVersion = getSupportedGLVersion();

    // delete the dummy
    destroyWindow(true);

    // update to the maximum opengl context
    if(glVersion[0] < m_config.opengl_major_version || (glVersion[0] == m_config.opengl_major_version && glVersion[1] < m_config.opengl_minor_version)) {
        m_config.opengl_major_version = glVersion[0];
        m_config.opengl_minor_version = glVersion[1];
    }

    m_firstInitialization = false;

    return true;
}

bool Engine::createWindow() {
    return (!m_firstInitialization && initSDL() == 0 && initGL() == 0);
}

int Engine::initSDL() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Engine::initSDL [SDL Init Error] : " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

    const int majorVersion = m_firstInitialization? 1 : m_config.opengl_major_version;
    const int minorVersion = m_firstInitialization? 4 : m_config.opengl_minor_version;

    // OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion);

	// Double Buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

    if(m_firstInitialization) {
        flags |= SDL_WINDOW_HIDDEN;
    }

    m_window = SDL_CreateWindow(
        m_config.appName.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_config.windowWidth, m_config.windowHeight,
        flags
    );

    if(m_window == 0) {
		std::cout << "Engine::initSDL [SDL Window Creation Error] : " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 2;
	}

    return 0;
}

int Engine::initGL() {
    // OpenGL context's creation
	m_oglContext = SDL_GL_CreateContext(m_window);

	if(m_oglContext == 0) {
		std::cout << "Engine::initGL [OpenGL Context Creation Error] : " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(m_window);
		SDL_Quit();
		return 3;
	}

    GLenum glew_init = glewInit();

    if(glew_init != GLEW_OK) {
        std::cout << "Engine::initGL [GLEW Init Error] : " << glewGetErrorString(glew_init) << std::endl;
        destroyWindow(true);
        return 4;
    }

	//Depth Buffer activation
	glEnable(GL_DEPTH_TEST);

	return 0;
}

std::string Engine::getGLVersion() const {
    unsigned int major = m_config.opengl_major_version;
    unsigned int minor = m_config.opengl_minor_version;
    return std::to_string(major) + "." + std::to_string(minor);
}

std::array<int, 2> Engine::getSupportedGLVersion() {
    std::string GL_version = std::string((char*)glGetString(GL_VERSION));
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

    return v;
}

void Engine::destroyWindow(bool sdlQuit) {
    SDL_GL_DeleteContext(m_oglContext);
    SDL_DestroyWindow(m_window);

    if(sdlQuit) {
        SDL_Quit();
    }
}

void Engine::loop() {
    while((m_stopOnWindowClose && m_window != NULL && !m_input.shouldClose(m_window)) || (m_running && m_window == NULL)) {        
        if(m_window) {
            m_input.updateEvents();
        }
        std::cout << "ok..." << std::endl;
    }
}

void Engine::terminateOnWindowClose() {
    m_stopOnWindowClose = true;
}





/* SIG HANDLER */
void Engine::SIGINT_handler(int s) {
    std::cout << "handled sigint" << std::endl;
}