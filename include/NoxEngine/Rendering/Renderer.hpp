/**
 * @copyright (c) 2026 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "NoxEngine/engine.typedef.hpp"
#include "NoxEngine/Rendering/Input.hpp"
#include "NoxEngine/Rendering/Scene.hpp"
#include "NoxEngine/Camera/Camera.hpp"
#include "NoxEngine/utils/IniSet.hpp"
#include "NoxEngine/utils/Color.hpp"

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

            Input* getInput() noexcept;

            void render(Scene& scene, Camera& camera);

            void show() noexcept;
            void hide() noexcept;
            void close() noexcept;
            bool shouldClose() const noexcept;
            bool isInitialized() const noexcept;

            void setFPS(const uint fps) noexcept;

            void setMouseGrab(const bool grabbed) noexcept;
            void setMouseFocus(const bool focus) noexcept;
            bool isMouseGrabbed() const noexcept;
            bool isMouseFocused() const noexcept;

            GLuint getCompactGLversion() const noexcept;
            V2D getSize() const noexcept;
            float getAspect() const noexcept;

            uint getFrameRate() const noexcept;
            float getTotalTimeElapsed() const noexcept;
            float getDeltaTime() const noexcept;

        protected:
            void clear(Color clearColor) noexcept;
            void swapWindow() noexcept;

            bool InitSDL();
            bool InitGL();
            void destroy();

            void updateInput() noexcept;

            bool m_isInit { false };
            bool m_shouldClose { false };
            IniSet m_config {};
            Input m_input {};
            SDL_Window* m_window { nullptr };
            SDL_GLContext m_glContext { 0 };
            VideoSettings m_settings {};
            VideoSettings m_maxCapabilities {};
	        Uint64 m_previousTime { 0 };
            uint m_frameRate { 0 };
            Uint32 m_targetFrameTime { 0 }; // Target time per frame in milliseconds
            float m_deltaTime { 0.0f }, m_totalTime { 0.0f };
            Color m_clearColor {};

        private:
            void loadHardwareCapabilities() noexcept;
    };

}

#endif // RENDERER_HPP