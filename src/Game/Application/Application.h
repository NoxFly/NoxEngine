#ifndef APPLICATION_H
#define APPLICATION_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "ResourceHolder.hpp"
#include "IniSet.h"
#include "Scene.h"
#include "Input.h"
#include "Shader.h"
#include "Texture.h"
#include "Drawable.h"
#include "MatricesMVP.h"

class Application {
	public:
		Application(IniSet& config, std::string basePath);
		~Application();

        void start();

        static std::string appBasePath;
        static std::string getPath(std::string filepath);

    protected:
        void compileShaders();
        void mainLoop();
        void update();
        bool isMouseFocused() const;
        void setMouseFocus(const bool focus);

        IniSet& m_config;
        const std::string m_basePath;
        int m_FPS;
        Scene m_scene;
        Input m_input;
        ResourceHolder<Shader, std::string> m_shaders;
        ResourceHolder<Texture, std::string> m_textures;
        MatricesMVP m_mvp;
        Drawable m_shape; // tmp
};

#endif // APPLICATION_H