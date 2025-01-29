/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "core/engine.typedef.hpp"
#include "core/Renderer/Input/Input.hpp"
#include "IniSet/IniSet.hpp"
#include "utils/Color.hpp"
#include "core/Scene/Scene.hpp"
#include "core/Camera/Camera.hpp"


namespace NoxEngine {

    struct VideoSettings {
        int antiAliasingLevel;
        int depthSize;
        int fps;
        int openglMajorVersion;
        int openglMinorVersion;
        bool hardwareAcceleration;
    };

    class Renderer {
        public:
            explicit Renderer(const IniSet& config);
            ~Renderer();

            void updateInput() noexcept;
            Input* getInput() noexcept;

            template <Dimension D>
            void render(Scene<D>& scene, Camera<D>& camera);

            void show() noexcept;
            void hide() noexcept;
            void close() noexcept;
            bool shouldClose() const noexcept;
            bool isInitialized() const noexcept;

            void setFPS(int fps) noexcept;
            
            void setMouseGrab(const bool grabbed) noexcept;
            void setMouseFocus(const bool focus) noexcept;
            bool isMouseGrabbed() const noexcept;
            bool isMouseFocused() const noexcept;

            GLuint getCompactGLversion() const noexcept;
            V2D getSize() const noexcept;
            float getAspect() const noexcept;

        protected:
            void clear(Color clearColor) noexcept;
            void swapWindow() noexcept;

            bool InitSDL();
            bool InitGL();
            void destroy();

            IniSet m_config;
            Input m_input;
            SDL_Window* m_window;
            SDL_GLContext m_glContext;
            bool m_isInit;
            bool m_shouldClose;
            VideoSettings m_settings;
            VideoSettings m_maxCapabilities;
            Uint32 m_earlyLoop, m_endLoop, m_spentTime;
            Color m_clearColor;

        private:
            void loadHardwareCapabilities() noexcept;
    };

}

#include "./Renderer.inl"

#endif // RENDERER_HPP