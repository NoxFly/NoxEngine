#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <functional>

#include "IniSet.h"
#include "Input.h"
#include "Video.h"
#include "utils.h"
#include "Camera.h"
#include "MatricesMVP.h"

class Scene {
	public:
        Scene();
		Scene(IniSet* config);
		~Scene();

        void show();
        void close();
        void updateView();
        void updatePlayerControls(const Input &input);
        void setDrawFunction(std::function<void()> drawFunction);
        void bindMatrices(MatricesMVP& mvp);

        bool isOpen() const;

        GLuint getCompactGLversion() const;
        Camera getCamera();
        Video* getVideo();

    private:
        void render();
        std::function<void()> draw;

        IniSet* m_config;
        bool m_running;
        Video m_video;
        Color m_clearColor;
        Camera m_camera;
        MatricesMVP* m_mvp;
};

#endif // SCENE_H