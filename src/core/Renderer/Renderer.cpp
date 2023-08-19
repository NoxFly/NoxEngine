//#pragma warning (disable: 4244)

#include "./Renderer.hpp"

#include <SDL3_image/SDL_image.h>
#include <iostream>

#include "Console/Console.hpp"
#include "core/Actor/Actor.hpp"
#include "utils/utils.hpp"


namespace NoxEngine {

    Renderer::Renderer(const IniSet& config):
        m_config(config),
        m_input(),
        m_window(0),
        m_glContext(0),
        m_isInit(false),
        m_shouldClose(false),
        m_FPS(30), m_earlyLoop(0), m_endLoop(0), m_spentTime(0),
        m_clearColor{}
    {
        if(m_config.hasKey("ENGINE", "background"))
            m_clearColor = getColorFromString(m_config.getValue("ENGINE", "background"));
        else
            m_clearColor = Color(0, 0, 0);

        if(InitSDL() && InitGL()) {
            m_isInit = true;
            // get a black screen before the rest of the code
            // loads a lot of things, and then we get a bugged window.
            swapWindow();

            Shader::setShadersPath(m_config.getValue("PATH", "shaders"));
            Shader::setDefaultGLSLversion(getCompactGLversion());
            Shader::loadFolder();

            Texture::setTexturesPath(m_config.getValue("PATH", "textures"));
            Geometry::setObjectsPath(m_config.getValue("PATH", "models"));
        }
    }

    Renderer::~Renderer() {
        destroy();
    }


    bool Renderer::InitSDL() {
        if(SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::string err = SDL_GetError();
            Console::error("Renderer::InitSDL", "Failed to init SDL : " + err);
            destroy();
            return false;
        }

        int sdl_maj_v = m_config.getIntValue("ENGINE", "OPENGL_MAJOR_VERSION", 1);
        int sdl_min_v = m_config.getIntValue("ENGINE", "OPENGL_MINOR_VERSION", 1);

        std::string title = m_config.getValue("ENGINE", "title");

        int fullscreenMode = m_config.getIntValue("ENGINE", "fullscreen", 0);
        int windowWidth = 0, windowHeight = 0;

        // fullscreen config :
        // 0 = not fullscreen (bug a bit)
        // 1 = fullscreen
        // 2 = fullscreen borderless

        // not fullscreen : default size
        if(fullscreenMode == 0) {
            windowWidth = m_config.getIntValue("ENGINE", "width", 640);
            windowHeight = m_config.getIntValue("ENGINE", "height", 480);
        }
        // fullscreen borderless
        else if(fullscreenMode == 2) {
            auto DM = SDL_GetCurrentDisplayMode(0);
            windowWidth = DM->w;
            windowHeight = DM->h;
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
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, m_config.getIntValue("ENGINE", "multisampling_buffer", 1));
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_config.getIntValue("ENGINE", "multisampling_amples", 16));

        m_window = SDL_CreateWindowWithPosition(
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
        if(m_config.hasKey("ENGINE", "icon")) {
            std::string imgPath = m_config.getValue("PATH", "images");
            std::string iconPath = ((imgPath[0] == '/')? "." : "") + imgPath + m_config.getValue("ENGINE", "icon");
            SDL_Surface* icon = IMG_Load(iconPath.c_str());

            if(icon != NULL) {
                SDL_SetWindowIcon(m_window, icon);
                SDL_DestroySurface(icon);
            }
            else {
                std::string err = SDL_GetError();
                Console::warn("Renderer::InitSDL", "Failed to set the window's icon : " + err);
            }
        }

        if(!m_config.getBoolValue("ENGINE", "hidden", false))
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

    // returns the opengl version as a string : for example, for version 1.1, returns "11"
    GLuint Renderer::getCompactGLversion() const noexcept {
        GLuint major = m_config.getIntValue("ENGINE", "OPENGL_MAJOR_VERSION", 1);
        GLuint minor = m_config.getIntValue("ENGINE", "OPENGL_MINOR_VERSION", 1);

        return (GLuint)std::stoul(std::to_string(major) + std::to_string(minor));
    }

    GLuint Renderer::getCompactGLversion() noexcept {
        GLuint major = m_config.getIntValue("ENGINE", "OPENGL_MAJOR_VERSION", 1);
        GLuint minor = m_config.getIntValue("ENGINE", "OPENGL_MINOR_VERSION", 1);

        return (GLuint)std::stoul(std::to_string(major) + std::to_string(minor));
    }

    void Renderer::updateInput() noexcept {
        m_input.updateEvents();

        if(m_input.shouldClose())
            close();
    }

    Input* Renderer::getInput() noexcept {
        return &m_input;
    }



    void Renderer::show() noexcept {
        SDL_ShowWindow(m_window);
    }


    void Renderer::hide() noexcept {
        SDL_HideWindow(m_window);
    }

    void Renderer::close() noexcept {
        m_shouldClose = true;
    }

    bool Renderer::shouldClose() const noexcept {
        return m_shouldClose;
    }

    bool Renderer::isInitialized() const noexcept {
        return m_isInit;
    }


    void Renderer::clear(Color clearColor) noexcept {
        float r = clearColor.r;
        float g = clearColor.g;
        float b = clearColor.b;
        float a = clearColor.a;

        // clear
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }


    void Renderer::swapWindow() noexcept {
        SDL_GL_SwapWindow(m_window);
    }


    void Renderer::setMouseGrab(const bool grabbed) noexcept {
        SDL_SetWindowGrab(m_window, grabbed? SDL_TRUE : SDL_FALSE);
    }


    void Renderer::setMouseFocus(const bool focus) noexcept {
        SDL_SetRelativeMouseMode(focus? SDL_TRUE : SDL_FALSE);
        V2D v = getSize();

        v.x /= 2;
        v.y /= 2;

        SDL_WarpMouseInWindow(m_window, v.x, v.y);
    }


    bool Renderer::isMouseGrabbed() const noexcept {
        return SDL_GetWindowGrab(m_window);
    }


    bool Renderer::isMouseFocused() const noexcept {
        return SDL_GetRelativeMouseMode();
    }

    V2D Renderer::getSize() const noexcept {
        int w, h;
        SDL_GetWindowSize(m_window, &w, &h);
        return V2D(w, h);
    }

    float Renderer::getAspect() const noexcept {
        V2D size = getSize();
        return size.x / size.y;
    }

    void Renderer::setFPS(int fps) noexcept {
        // fps no limit
        if(fps < 0)
            fps = -1;
        
        m_FPS = fps;
    }

}