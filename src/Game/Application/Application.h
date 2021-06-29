#ifndef APPLICATION_H
#define APPLICATION_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "IniSet.h"
#include "Scene.h"
#include "Input.h"
#include "Block.h"
#include "MatricesMVP.h"
#include "World.h"

class Application {
	public:
		Application(IniSet& config, std::string basePath);
		~Application();

        void start();

        static std::string appBasePath;
        static std::string getPath(std::string filepath);

    protected:
        void compileShaders();
        void loadTextures();
        void mainLoop();
        void update();
        bool isMouseFocused() const;
        void setMouseFocus(const bool focus);

        IniSet& m_config;
        const std::string m_basePath;
        int m_FPS;
        Scene m_scene;
        Input m_input;
        MatricesMVP m_mvp;
        World m_world;
};

#endif // APPLICATION_H