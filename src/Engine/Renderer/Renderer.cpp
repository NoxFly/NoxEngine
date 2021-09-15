#include "Renderer.h"

#include <SDL2/SDL_image.h>
#include <iostream>

#include "Console.h"


Renderer::Renderer(IniSet& config):
    m_input(),
    m_window(0),
    m_glContext(0),
    m_isInit(false),
    m_shouldClose(false),
    m_FPS(30), m_earlyLoop(0), m_endLoop(0), m_spentTime(0),
    m_clearColor{0,0,0}
{
    if(config.hasKey("WINDOW", "background"))
        m_clearColor = getColorFromString(config.getValue("WINDOW", "background"));
    else
        m_clearColor = Color(0, 0, 0);

    if(InitSDL(config) && InitGL())
        m_isInit = true;
}

Renderer::~Renderer() {
    destroy();
}


bool Renderer::InitSDL(IniSet& config) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::string err = SDL_GetError();
		Console::error("Renderer::InitSDL", "Failed to init SDL : " + err);
		destroy();
		return false;
	}

    int sdl_maj_v = config.getIntValue("WINDOW", "OPENGL_MAJOR_VERSION", 1);
    int sdl_min_v = config.getIntValue("WINDOW", "OPENGL_MINOR_VERSION", 1);

    std::string title = config.getValue("WINDOW", "title");

    int fullscreenMode = config.getIntValue("WINDOW", "fullscreen", 0);
    int windowWidth = 0, windowHeight = 0;

    // fullscreen config :
    // 0 = not fullscreen (bug a bit)
    // 1 = fullscreen
    // 2 = fullscreen borderless

    // not fullscreen : default size
    if(fullscreenMode == 0) {
        windowWidth = config.getIntValue("WINDOW", "width", 640);
        windowHeight = config.getIntValue("WINDOW", "height", 480);
    }
    // fullscreen borderless
    else if(fullscreenMode == 2) {
        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);
        windowWidth = DM.w;
        windowHeight = DM.h;
    }

#ifdef DEBUG
    std::cout
        <<"Init SDL with opengl version " << sdl_maj_v << "." << sdl_min_v
        << ", window size " << windowWidth << "x" << windowHeight
        << ", fullscreen mode = " << (fullscreenMode==0? "windowed" : fullscreenMode==1? "fullscreen" : "borderless")
        << std::endl;
#endif

	// OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, sdl_maj_v);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, sdl_min_v);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Double Buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // multi-sampling
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, config.getIntValue("WINDOW", "multisampling_buffer", 1));
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config.getIntValue("WINDOW", "multisampling_amples", 16));

	m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL
    );

	if(m_window == 0) {
        std::string err = SDL_GetError();
		Console::error("Renderer::InitSDL", "Failed to create the window : " + err);
		destroy();
		return false;
	}

    if(fullscreenMode == 1)
        SDL_SetWindowFullscreen(m_window, SDL_TRUE);

	// set window's icon
    if(config.hasKey("WINDOW", "icon")) {
        std::string iconPath = "." + config.getValue("PATH", "images") + config.getValue("WINDOW", "icon");
        SDL_Surface* icon = IMG_Load(iconPath.c_str());

        if(icon != NULL) {
            SDL_SetWindowIcon(m_window, icon);
            SDL_FreeSurface(icon);
        }
        else {
            std::string err = SDL_GetError();
            Console::warn("Renderer::InitSDL", "Failed to set the window's icon : " + err);
        }
    }

    if(!config.getBoolValue("WINDOW", "hidden", false))
        show();

    return true;
}


bool Renderer::InitGL() {
    // OpenGL context's creation
	m_glContext = SDL_GL_CreateContext(m_window);

	// error initializing openGL = qui window
	if(m_glContext == 0) {
        std::string err = SDL_GetError();
		Console::error("Renderer::InitGL", "Failed to create SDL_GL context : " + err);
		destroy();
		return false;
	}

    // GLEW init
    GLenum initialisationGLEW(glewInit());

    // fail : quit window
    if(initialisationGLEW != GLEW_OK) {
        std::string err = (char*)glewGetErrorString(initialisationGLEW);
        Console::error("Renderer::InitGL", "Failed to init GLEW : " + err);
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


void Renderer::destroy() {
    if(m_glContext != 0)
        SDL_GL_DeleteContext(m_glContext);
    
    if(m_window != 0)
        SDL_DestroyWindow(m_window);
    
    SDL_Quit();
}

void Renderer::updateInput() {
    m_input.updateEvents();

    if(m_input.shouldClose())
        close();
}


void Renderer::show() {
    SDL_ShowWindow(m_window);
}


void Renderer::hide() {
    SDL_HideWindow(m_window);
}

void Renderer::close() {
    m_shouldClose = true;
}

bool Renderer::shouldClose() const {
    return m_shouldClose;
}

bool Renderer::isInitialized() const {
    return m_isInit;
}


void Renderer::clear(Color clearColor) {
    float r = clearColor.r / 255.0f;
    float g = clearColor.g / 255.0f;
    float b = clearColor.b / 255.0f;
    float a = clearColor.a / 255.0f;

    // clear
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Renderer::swapWindow() {
    SDL_GL_SwapWindow(m_window);
}


void Renderer::setMouseGrab(const bool grabbed) {
    SDL_SetWindowGrab(m_window, grabbed? SDL_TRUE : SDL_FALSE);
}


void Renderer::setMouseFocus(const bool focus) {
    SDL_SetRelativeMouseMode(focus? SDL_TRUE : SDL_FALSE);
    glm::vec2 v = getSize();
    SDL_WarpMouseInWindow(m_window, v.x / 2, v.y / 2);
}


bool Renderer::isMouseGrabbed() const {
    return SDL_GetWindowGrab(m_window);
}


bool Renderer::isMouseFocused() const {
    return SDL_GetRelativeMouseMode();
}

glm::vec2 Renderer::getSize() const {
    int w, h;
    SDL_GetWindowSize(m_window, &w, &h);
    return glm::vec2(w, h);
}

float Renderer::getAspect() const {
    glm::vec2 size = getSize();
    return size.x / size.y;
}

void Renderer::setFPS(int fps) {
    // fps no limit
    if(fps < 0)
        fps = -1;
    
    m_FPS = fps;
}

void Renderer::render(Scene& scene, Camera& camera) {
    Uint32 frameRate = 1000 / m_FPS;
    m_earlyLoop = SDL_GetTicks();


    clear(m_clearColor);

    // render the scene through camera
    (void)scene;
    (void)camera;

    swapWindow();


    m_endLoop = SDL_GetTicks();
    m_spentTime = m_endLoop - m_earlyLoop;

    if(m_FPS == -1 && m_spentTime < frameRate)
        SDL_Delay(frameRate - m_spentTime);
}