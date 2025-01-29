//#pragma warning (disable: 4244)

#include "./Renderer.hpp"

#include <SDL2/SDL_image.h>
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
        m_settings{},
        m_maxCapabilities{},
        m_earlyLoop(0), m_endLoop(0), m_spentTime(0),
        m_clearColor{}
    {
        m_clearColor = (m_config.hasKey("ENGINE", "background"))
            ? getColorFromString(m_config.getValue("ENGINE", "background"))
            : Color(0, 0, 0);

        if(!InitSDL() || !InitGL()) {
            exit(1);
        }

        m_isInit = true;
        // get a black screen before the rest of the code
        // loads a lot of things, and then we get a bugged window.
        swapWindow();

        Shader::setShadersPath(m_config.getValue("PATH", "shaders"));
        Shader::setDefaultGLSLversion(getCompactGLversion());
        Shader::loadFolder();

        Texture::setTexturesPath(m_config.getValue("PATH", "textures"));
        Actor<V3D>::setObjectsPath(m_config.getValue("PATH", "models"));
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

        std::string title = m_config.getValue("ENGINE", "title");

        int fullscreenMode = m_config.getIntValue("ENGINE", "fullscreen", 0);
        int windowWidth = 0, windowHeight = 0;
        Uint32 flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL;

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
            SDL_DisplayMode DM;
            SDL_GetCurrentDisplayMode(0, &DM);
            windowWidth = DM.w;
            windowHeight = DM.h;
            flags |= SDL_WINDOW_BORDERLESS;
        }

        loadHardwareCapabilities();

#ifdef DEBUG
        std::cout
            <<"Init SDL with opengl version " << m_settings.openglMajorVersion << "." << m_settings.openglMinorVersion
            << ", window size " << windowWidth << "x" << windowHeight
            << ", fullscreen mode = " << (fullscreenMode==0? "windowed" : fullscreenMode==1? "fullscreen" : "borderless")
            << std::endl;
#endif


        // OpenGL version
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_settings.openglMajorVersion);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_settings.openglMinorVersion);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        // Double Buffer
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // 1=enable, should always be 1
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_settings.depthSize); // (low) 16, (medium) 24, (hight) 32

        // Anti-aliasing
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, m_settings.hardwareAcceleration && m_settings.antiAliasingLevel > 0); // 1=enabled
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_settings.antiAliasingLevel); // (none) 0, (low) 2, (medium) 4, (high) 8, (ultra) 16

        m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, flags);

        if(m_window == 0) {
            std::string err = SDL_GetError();
            Console::error("Renderer::InitSDL", "Failed to create the window : " + err);
            destroy();
            return false;
        }

        if(fullscreenMode == 1) {
            SDL_SetWindowFullscreen(m_window, SDL_TRUE);
        }

        // set window's icon
        if(m_config.hasKey("ENGINE", "icon")) {
            std::string imgPath = m_config.getValue("PATH", "images");
            std::string iconPath = ((imgPath[0] == '/')? "." : "") + imgPath + m_config.getValue("ENGINE", "icon");
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

        if(!m_config.getBoolValue("ENGINE", "hidden", false)) {
            show();
        }

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
        GLenum initializationGLEW(glewInit());

        // fail : quit window
        if(initializationGLEW != GLEW_OK) {
            std::string err = (char*)glewGetErrorString(initializationGLEW);
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
        GLuint major = m_settings.openglMajorVersion;
        GLuint minor = m_settings.openglMinorVersion;

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
        SDL_SetWindowMouseGrab(m_window, grabbed? SDL_TRUE : SDL_FALSE);
    }


    void Renderer::setMouseFocus(const bool focus) noexcept {
        SDL_SetRelativeMouseMode(focus? SDL_TRUE : SDL_FALSE);
        V2D v = getSize();

        v.x /= 2;
        v.y /= 2;

        SDL_WarpMouseInWindow(m_window, v.x, v.y);
    }


    bool Renderer::isMouseGrabbed() const noexcept {
        return SDL_GetWindowMouseGrab(m_window);
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
        // -1 = no limit
        m_settings.fps = std::clamp(fps, -1, m_maxCapabilities.fps);
    }


    void Renderer::loadHardwareCapabilities() noexcept {
        SDL_Window* tempWindow = SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        SDL_Renderer* tempRenderer = SDL_CreateRenderer(tempWindow, -1, 0);

        SDL_RendererInfo rendererInfo;
        SDL_GetRendererInfo(tempRenderer, &rendererInfo);

        int multiSampleBuffers = 0;

        glGetIntegerv(GL_MULTISAMPLE, &multiSampleBuffers);
        glGetIntegerv(GL_MAX_SAMPLES, &m_maxCapabilities.antiAliasingLevel);
        glGetIntegerv(GL_MAJOR_VERSION, &m_maxCapabilities.openglMajorVersion);
        glGetIntegerv(GL_MAJOR_VERSION, &m_maxCapabilities.openglMajorVersion);
        glGetIntegerv(GL_DEPTH_BITS, &m_maxCapabilities.depthSize);

        m_maxCapabilities.hardwareAcceleration = multiSampleBuffers > 0;

        // get monitor max refresh rate
        SDL_DisplayMode DM;
        int displayIndex = SDL_GetWindowDisplayIndex(tempWindow);
        SDL_GetCurrentDisplayMode(displayIndex, &DM);
        m_maxCapabilities.fps = DM.refresh_rate;

        SDL_DestroyRenderer(tempRenderer);
        SDL_DestroyWindow(tempWindow);

        auto oglMajor                   = m_config.getIntValue("ENGINE", "opengl_major_version", 3);
        auto oglMinor                   = m_config.getIntValue("ENGINE", "opengl_minor_version", 1);
        auto aaLevel                    = m_config.getIntValue("ENGINE", "multisampling_level", 2);
        auto depthSize                  = m_config.getIntValue("ENGINE", "depth_size", 16);
        auto askHardwareAcceleration    = m_config.getBoolValue("ENGINE", "hardware_acceleration", true);
        auto fps                        = m_config.getIntValue("ENGINE", "fps", 30);

        m_settings.openglMajorVersion   = std::clamp(oglMajor, 1, m_maxCapabilities.openglMajorVersion);
        m_settings.openglMinorVersion   = std::clamp(oglMinor, 0, m_maxCapabilities.openglMinorVersion);
        m_settings.hardwareAcceleration = m_maxCapabilities.hardwareAcceleration && askHardwareAcceleration;
        m_settings.antiAliasingLevel    = std::clamp(aaLevel, 0, m_maxCapabilities.antiAliasingLevel);
        m_settings.depthSize            = std::clamp(depthSize, 16, 32);
        m_settings.fps                  = std::clamp(fps, -1, m_maxCapabilities.fps);

        Console::info("----------- Hardware max capabilities -----------");
        Console::info("OpenGL version\t\t"          + std::to_string(m_maxCapabilities.openglMajorVersion) + "." + std::to_string(m_maxCapabilities.openglMinorVersion));
        Console::info("Hardware acceleration\t"     + std::string(m_maxCapabilities.hardwareAcceleration ? "available" : "not available"));
        Console::info("Multisampling level\t"       + std::to_string(m_maxCapabilities.antiAliasingLevel));
        Console::info("Depth buffer size\t"         + std::to_string(m_maxCapabilities.depthSize));
        Console::info("Max refresh rate\t"          + std::to_string(m_maxCapabilities.fps) + " Hz");
        Console::info("-------------------------------------------------");

        Console::info("------------- Applied configuration -------------");
        Console::info("OpenGL version\t\t"          + std::to_string(m_settings.openglMajorVersion) + "." + std::to_string(m_settings.openglMinorVersion));
        Console::info("Hardware acceleration\t"     + std::string(m_settings.hardwareAcceleration? "enabled" : "disabled"));
        Console::info("Multisampling level\t"       + std::to_string(m_settings.antiAliasingLevel));
        Console::info("Depth buffer size\t"         + std::to_string(m_settings.depthSize));
        Console::info("fps\t\t\t"                   + std::to_string(m_settings.fps));
        Console::info("-------------------------------------------------");
    }

}