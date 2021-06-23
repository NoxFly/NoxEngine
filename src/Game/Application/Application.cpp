#include "Application.h"

#include <future>
#include <SDL2/SDL.h>
#include <filesystem>

#include "Shader.h"
#include "Console.h"

namespace fs = std::filesystem;

std::string Application::appBasePath = "./";

Application::Application(IniSet& config, std::string basePath):
    m_config(config),
    m_basePath(basePath),
    m_scene(&config),
    m_input(),
    m_mvp(70.0, (double)(config.getFloatValue("WINDOW", "width", 1) / config.getFloatValue("WINDOW", "height", 1)), 1.0, 100.0),
    
    // tmp
    m_shape(
        new float[9]{ -0.5, -0.5, -1.0,   0.0, 0.5, -1.0,   0.5, -0.5, -1.0 },
        new float[9]{  1.0,  0.0,  0.0,   0.0, 1.0,  0.0,   0.0,  0.0,  1.0 },
        9
    )
    //
{
    Application::appBasePath = basePath;

    Shader::setShadersPath(Application::getPath(config.getValue("PATH", "shaders")));
    Drawable::setShadersBank(m_shaders);

    m_scene.bindMatrices(m_mvp);
    m_scene.getCamera().lookAt(glm::vec3(3, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

Application::~Application() {

}


void Application::start() {
    // compile shaders asynchronously
    auto futureCompileShaders = std::async([this]() {
        compileShaders();
        m_shape.setShader("color3D");
    });

    // capture the mouse
    m_scene.getVideo()->setMouseFocus(true);

    // tell the scene what's to draw
    m_scene.setDrawFunction([this]() {
        m_shape.draw(m_mvp.getMVP());
    });

    // and start the main loop
    mainLoop();
}


void Application::mainLoop() {
    m_scene.show();

    while(m_scene.isOpen()) {
        update();
        m_scene.updateView(m_input);
    }
}


void Application::update() {
    m_input.updateEvents();

    if(m_input.shouldClose())
        m_scene.close();

    // useless to update things if the window should close
    else {
        // Esc : stop capture the mouse
        if(m_input.isKeyDown(SDL_SCANCODE_ESCAPE)) {
            if(m_scene.getVideo()->isMouseFocused())
                m_scene.getVideo()->setMouseFocus(false);
        }

        // if mouse isn't captured and a click is performed on the window,
        // capture it back
        if(m_input.isMouseButtonDown(SDL_BUTTON_LEFT)) {
            if(!m_scene.getVideo()->isMouseFocused())
                m_scene.getVideo()->setMouseFocus(true);
        }

        // update game's controls only if the mouse is captured
        if(m_scene.getVideo()->isMouseFocused())
            m_scene.updatePlayerControls(m_input);


        // update MVP matrix
        m_mvp.updateMVP();
    }
}


void Application::compileShaders() {
    // create fake gl context to compile shaders
    SDL_GL_CreateContext(SDL_CreateWindow("", 0, 0, 0, 0, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL));

    std::string shadersDir = Application::getPath(m_config.getValue("PATH", "shaders"));
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
            Shader shader(key, m_scene.getCompactGLversion());

            // try to compile it
            if(shader.load()) {
                m_shaders.set(key, shader);
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


std::string Application::getPath(std::string filepath) {
    return Application::appBasePath + filepath;
}