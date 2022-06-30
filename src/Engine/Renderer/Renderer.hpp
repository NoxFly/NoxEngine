#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Input.hpp"
#include "IniSet.hpp"
#include "Color.hpp"
#include "Scene.hpp"
#include "Camera.hpp"


class Renderer {
	public:
		Renderer(IniSet& config);
		~Renderer();

        void updateInput();
        Input* getInput();
        void render(Scene* scene, Camera* camera);

        void show();
        void hide();
        void close();
        bool shouldClose() const;
        bool isInitialized() const;

        void setFPS(int fps);
        
        void setMouseGrab(const bool grabbed);
        void setMouseFocus(const bool focus);
        bool isMouseGrabbed() const;
        bool isMouseFocused() const;

        GLuint getCompactGLversion() const;
        GLuint getCompactGLversion();
        glm::vec2 getSize() const;
        float getAspect() const;

    protected:
        void clear(Color clearColor);
        void swapWindow();

        bool InitSDL();
        bool InitGL();
        void destroy();

        IniSet m_config;
        Input m_input;
        SDL_Window* m_window;
        SDL_GLContext m_glContext;
        bool m_isInit;
        bool m_shouldClose;
        int m_FPS;
        Uint32 m_earlyLoop, m_endLoop, m_spentTime;
        Color m_clearColor;
};

#endif // RENDERER_HPP