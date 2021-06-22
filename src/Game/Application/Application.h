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

        IniSet& config;
        const std::string basePath;
        Scene scene;
        Input input;
        ResourceHolder<Shader, std::string> shaders;
        ResourceHolder<Texture, std::string> textures;
        glm::mat4 projection, modelview, saveModelview;
        Drawable shape; // tmp
};

#endif // APPLICATION_H