// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/platform/Window.hpp>
#include <NoxEngine/core/Assert.hpp>
#include <NoxEngine/core/Logger.hpp>

#include <GL/glew.h>
#include <SDL3/SDL.h>

#ifdef NOX_HAS_IMGUI
#include <imgui_impl_sdl3.h>
#endif

#include <stdexcept>
#include <string>

namespace Nox {

    Window::Window(std::string_view title, int width, int height)
        : width_(width)
        , height_(height) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
        }

        // Request OpenGL 4.6 core profile
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    #ifndef NDEBUG
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    #endif

        window_ = SDL_CreateWindow(
            std::string(title).c_str(),
            width_, height_,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );

        if (!window_) {
            throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
        }

        glContext_ = SDL_GL_CreateContext(window_);
        if (!glContext_) {
            throw std::runtime_error(std::string("SDL_GL_CreateContext failed: ") + SDL_GetError());
        }

        SDL_GL_MakeCurrent(window_, static_cast<SDL_GLContext>(glContext_));

        // Initialize GLEW
        glewExperimental = GL_TRUE;
        GLenum glewErr = glewInit();
        if (glewErr != GLEW_OK) {
            throw std::runtime_error(std::string("glewInit failed: ") +
                reinterpret_cast<const char*>(glewGetErrorString(glewErr)));
        }

        // V-Sync on by default
        SDL_GL_SetSwapInterval(1);

        NOX_LOG_INFO("OpenGL {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
        NOX_LOG_INFO("Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    }

    Window::~Window() {
        if (glContext_) {
            SDL_GL_DestroyContext(static_cast<SDL_GLContext>(glContext_));
        }
        if (window_) {
            SDL_DestroyWindow(window_);
        }
        SDL_Quit();
    }

    bool Window::pollEvents() {
        input_.endFrame();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
#ifdef NOX_HAS_IMGUI
            ImGui_ImplSDL3_ProcessEvent(&event);
#endif
            input_.processEvent(event);

            switch (event.type) {
                case SDL_EVENT_QUIT:
                    onClose.emit();
                    return false;
                case SDL_EVENT_WINDOW_RESIZED:
                    width_  = event.window.data1;
                    height_ = event.window.data2;
                    onResize.emit(width_, height_);
                    break;
                default:
                    break;
            }
        }
        return true;
    }

    void Window::swapBuffers() {
        SDL_GL_SwapWindow(window_);
    }

    float Window::aspect() const {
        if (height_ == 0) return 1.0f;
        return static_cast<float>(width_) / static_cast<float>(height_);
    }

    void Window::setRelativeMouseMode(bool enabled) {
        relativeMouseMode_ = enabled;
        SDL_SetWindowRelativeMouseMode(window_, enabled);
        if (!enabled) {
            // Show and un-lock the cursor when leaving relative mode
            SDL_ShowCursor();
        }
        else {
            SDL_HideCursor();
        }
    }

} // namespace Nox
