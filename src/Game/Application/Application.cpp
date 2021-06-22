#include "Application.h"

#include <future>
#include <SDL2/SDL.h>
#include <filesystem>

#include "Shader.h"
#include "Console.h"

namespace fs = std::filesystem;

std::string Application::appBasePath = "./";

Application::Application(IniSet& config, std::string basePath):
    config(config),
    basePath(basePath),
    scene(&config),
    input(),
    shape(
        new float[9]{ -0.5, -0.5, -1.0,   0.0, 0.5, -1.0,   0.5, -0.5, -1.0 },
        new float[9]{  1.0,  0.0,  0.0,   0.0, 1.0,  0.0,   0.0,  0.0,  1.0 },
        9
    )
{
    Application::appBasePath = basePath;

    Shader::setShadersPath(Application::getPath(config.getValue("PATH", "shaders")));
    Drawable::setShadersBank(shaders);

    double aspect = (double)(config.getFloatValue("WINDOW", "width", 0) / config.getFloatValue("WINDOW", "height", 0));
    projection = glm::perspective(70.0, aspect, 1.0, 100.0);
	modelview = glm::mat4(1.0);
    saveModelview = modelview;

	// actualization if the view in the matrice
	modelview = glm::lookAt(glm::vec3(3, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
    
}

Application::~Application() {

}

void Application::start() {
    auto futureCompileShaders = std::async([this]() {
        compileShaders();
        shape.setShader("color3D");
    });

    scene.bindMatrixes(projection, modelview, saveModelview);

    scene.getVideo()->setMouseFocus(true);

    scene.setDrawFunction([this]() {
        shape.draw(projection, modelview);
    });

    mainLoop();
}

void Application::mainLoop() {
    scene.show();

    while(scene.isOpen()) {
        update();
        scene.updateView(input, projection, modelview);
    }
}

void Application::compileShaders() {
    // create fake gl context to compile shaders
    SDL_GL_CreateContext(SDL_CreateWindow("", 0, 0, 0, 0, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL));

    std::string shadersDir = Application::getPath(config.getValue("PATH", "shaders"));
    std::map<std::string, std::pair<bool, bool>> shadersName = {};

    // read shaders dir and compile all of them
    for(const auto& entry : fs::directory_iterator(shadersDir)) {
        if(entry.exists()) {
            std::string fullname = entry.path().string().substr(shadersDir.size());
            size_t extPos = fullname.find_last_of('.');
            std::string ext = fullname.substr(extPos+1);
            std::string name = fullname.substr(0, extPos);

            bool extIsFrag = ext == "frag";
            bool extIsVert = ext == "vert";

            if(extIsFrag || extIsVert) {
                // not listed, add it for the first time
                if(shadersName.count(name) == 0)
                    shadersName[name] = std::pair(extIsFrag, extIsVert);
                // already seen 1 time, update
                else {
                    if(extIsFrag) shadersName[name].first = true;
                    if(extIsVert) shadersName[name].second = true;
                }
            }
        }

    }

    // iterate all possible shader pairs
    for(auto const& [key, val] : shadersName) {
        // if complete shader
        if(val.first && val.second) {
            Shader shader(key, scene.getCompactGLversion());

            // try to compile it
            if(shader.load()) {
                shaders.set(key, shader);
#ifdef DEBUG
                Console::info("Compiled shader '" + key + "'");
#endif
            }
#ifdef DEBUG
            else
                Console::error("Failed to compile shader '" + key + "'");
#endif
        }
    }
}

void Application::update() {
    input.updateEvents();

    if(input.shouldClose())
        scene.close();

    // useless to update things if the 
    else {
        if(input.isKeyDown(SDL_SCANCODE_ESCAPE)) {
            if(scene.getVideo()->isMouseFocused())
                scene.getVideo()->setMouseFocus(false);
        }

        if(input.isMouseButtonDown(SDL_BUTTON_LEFT)) {
            if(!scene.getVideo()->isMouseFocused())
                scene.getVideo()->setMouseFocus(true);
        }

        if(scene.getVideo()->isMouseFocused())
            scene.updatePlayerControls(input);
    }
}

std::string Application::getPath(std::string filepath) {
    return Application::appBasePath + filepath;
}