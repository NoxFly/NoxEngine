#include "Video.h"

#include <SDL2/SDL_image.h>
#include <iostream>

#include "Console.h"
#include "Application.h"

Video::Video():
    window(0), glContext(0), isInit(false)
{

}

Video::Video(IniSet& config, const std::string& section):
    Video()
{
    if(InitSDL(config, section) && InitGL(config, section))
        isInit = true;
}

Video::~Video() {
    destroy();
}

bool Video::InitSDL(IniSet& config, const std::string& section) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::string err = SDL_GetError();
		Console::error("Video::InitSDL", "Failed to init SDL : " + err);
		destroy();
		return false;
	}

    int sdl_maj_v = config.getIntValue(section, "OPENGL_MAJOR_VERSION", 1);
    int sdl_min_v = config.getIntValue(section, "OPENGL_MINOR_VERSION", 1);

    std::string title = config.getValue(section, "title");

    int fullscreenMode = config.getIntValue(section, "fullscreen", 0);
    int windowWidth = 0, windowHeight = 0;

    // fullscreen config :
    // 0 = not fullscreen (bug a bit)
    // 1 = fullscreen
    // 2 = fullscreen borderless

    // not fullscreen : default size
    if(fullscreenMode == 0) {
        windowWidth = config.getIntValue(section, "width", 640);
        windowHeight = config.getIntValue(section, "height", 480);
    }
    // fullscreen borderless
    else if(fullscreenMode == 2) {
        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);
        windowWidth = DM.w;
        windowHeight = DM.h;
    }

#ifdef DEBUG
    printf("Init SDL with opengl version %d.%d, window size %dx%d, fullscreen mode = %s\n",
        sdl_maj_v, sdl_min_v, windowWidth, windowHeight,
        fullscreenMode==0? "windowed" : fullscreenMode==1? "fullscreen" : "borderless"
    );
#endif

	// OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, sdl_maj_v);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, sdl_min_v);

	// Double Buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL
    );

	if(window == 0) {
        std::string err = SDL_GetError();
		Console::error("Video::InitSDL", "Failed to create the window : " + err);
		destroy();
		return false;
	}

    if(fullscreenMode == 1)
        SDL_SetWindowFullscreen(window, SDL_TRUE);

	// set window's icon
    if(config.hasKey(section, "icon")) {
        std::string iconPath = config.getValue("PATH", "images") + config.getValue(section, "icon");
        SDL_Surface* icon = IMG_Load(Application::getPath(iconPath).c_str());

        if(icon != NULL) {
            SDL_SetWindowIcon(window, icon);
            SDL_FreeSurface(icon);
        }
        else {
            std::string err = SDL_GetError();
            Console::warn("Video::InitSDL", "Failed to set the window's icon : " + err);
        }
    }

    return true;
}

bool Video::InitGL(IniSet& config, const std::string& section) {
    // OpenGL context's creation
	glContext = SDL_GL_CreateContext(window);

	// error initializing openGL = qui window
	if(glContext == 0) {
        std::string err = SDL_GetError();
		Console::error("Video::InitGL", "Failed to create SDL_GL context : " + err);
		destroy();
		return false;
	}

    // GLEW init
    GLenum initialisationGLEW(glewInit());

    // fail : quit window
    if(initialisationGLEW != GLEW_OK) {
        std::string err = (char*)glewGetErrorString(initialisationGLEW);
        Console::error("Video::InitGL", "Failed to init GLEW : " + err);
        // quit SDL
        destroy();

        return false;
    }

	//Depth Buffer activation
	glEnable(GL_DEPTH_TEST);
    // anti-aliasing
    glEnable(GL_MULTISAMPLE);

    return true;
}

void Video::destroy() {
    if(glContext != 0)
        SDL_GL_DeleteContext(glContext);
    
    if(window != 0)
        SDL_DestroyWindow(window);
    
    SDL_Quit();
}

void Video::show() {
    SDL_ShowWindow(window);
}

void Video::hide() {
    SDL_HideWindow(window);
}

bool Video::isInitialized() const {
    return isInit;
}

void Video::clear(Color clearColor) {
    float r = clearColor.r / 255.0f;
    float g = clearColor.g / 255.0f;
    float b = clearColor.b / 255.0f;
    float a = clearColor.a / 255.0f;

    // clear
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Video::swapWindow() {
    SDL_GL_SwapWindow(window);
}

void Video::setMouseGrab(const bool grabbed) {
    SDL_SetWindowGrab(window, grabbed? SDL_TRUE : SDL_FALSE);
}

void Video::setMouseFocus(const bool focus) {
    SDL_SetRelativeMouseMode(focus? SDL_TRUE : SDL_FALSE);
}

bool Video::isMouseGrabbed() const {
    return SDL_GetWindowGrab(window);
}

bool Video::isMouseFocused() const {
    return SDL_GetRelativeMouseMode();
}
