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

class Scene {
	public:
        Scene();
		Scene(IniSet* config);
		~Scene();

        void show();
        void close();
        void updateView(const Input &input, glm::mat4& projection, glm::mat4& modelview);
        void updatePlayerControls(const Input &input);
        void setDrawFunction(std::function<void()> drawFunction);
        void bindMatrixes(glm::mat4& projection, glm::mat4& modelview, glm::mat4& saveModelview);

        bool isOpen() const;

        GLuint getCompactGLversion() const;
        Camera getCamera();
        Video* getVideo();

    private:
        void render(glm::mat4& projection, glm::mat4& modelview);
        std::function<void()> draw;

        IniSet* config;
        bool running;
        int fps;
        Video video;
        Color clearColor;
        Uint32 earlyLoop, endLoop, spentTime;
        Camera camera;
        glm::mat4 *projection, *modelview, *saveModelview;
};

#endif // SCENE_H