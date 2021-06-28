#include "Application.h"

#include <SDL2/SDL.h>
#include <filesystem>

#include "Shader.h"
#include "Texture.h"
#include "resourceBank.h"
#include "Console.h"

namespace fs = std::filesystem;


std::string Application::appBasePath = "./";

ResourceHolder<Shader, std::string> shadersBank;
ResourceHolder<Texture, std::string> texturesBank;


Application::Application(IniSet& config, std::string basePath):
    m_config(config),
    m_basePath(basePath),
    m_FPS(config.getIntValue("WINDOW", "fps", 30)),
    m_scene(&config),
    m_input(),
    m_mvp(
        glm::radians(70.0f),
        (double)(config.getFloatValue("WINDOW", "width", 1) / config.getFloatValue("WINDOW", "height", 1)),
        0.1f,
        100.0f
    ),
    // tmp
    m_shape()
{
    Application::appBasePath = basePath;

    Shader::setShadersPath(Application::getPath(config.getValue("PATH", "shaders")));

    m_scene.bindMatrices(m_mvp);
    m_input.setCurrentWindow(*m_scene.getVideo());
    m_scene.getCamera().lookAt(glm::vec3(2, 2, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

Application::~Application() {

}


void Application::start() {
    m_scene.show();

    compileShaders();
    loadTextures();

    m_shape = Block(0, 0, 0, "grass_bottom");

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

    Uint32 earlyLoop(0), endLoop(0), spentTime(0);

    while(m_scene.isOpen()) {
        Uint32 frameRate = 1000 / m_FPS;
        earlyLoop = SDL_GetTicks();


        update();
        m_scene.updateView();


        endLoop = SDL_GetTicks();
        spentTime = endLoop - earlyLoop;

        if(spentTime < frameRate)
            SDL_Delay(frameRate - spentTime);
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
            std::unique_ptr<Shader> shader = std::make_unique<Shader>(key, m_scene.getCompactGLversion());

            // try to compile it
            if(shader->load())
                shadersBank.set(key, std::move(shader));
#ifdef DEBUG
            else
                Console::error("Failed to compile shader '" + key + "'");
#endif
        }
    }
}


void Application::loadTextures() {
    std::string texDir = Application::getPath(m_config.getValue("PATH", "textures") + "blocks/");

    // read shaders dir and compile all of them
    for(const auto& entry : fs::directory_iterator(texDir)) {
        if(entry.exists()) {
            if(entry.is_regular_file()) {
                std::string fullname = entry.path().string().substr(texDir.size());
                size_t extPos = fullname.find_last_of('.');
                std::string ext = fullname.substr(extPos+1);
                std::string name = fullname.substr(0, extPos);

                if(ext == "png" || ext == "jpg") {
                    std::unique_ptr<Texture> tex = std::make_unique<Texture>(texDir + fullname, name);

                    if(tex->load())
                        texturesBank.set(name, std::move(tex));
#ifdef DEBUG
                    else
                        Console::info("Failed to load texture " + name);
#endif
                }
            }
        }
    }
}


std::string Application::getPath(std::string filepath) {
    return Application::appBasePath + filepath;
}