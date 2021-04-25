#include <iostream>

#include "Engine.h"
#include "Shader.h"

void update(Engine& engine);
void render(Engine& engine);

int main(int argc, char **argv) {
    // initialize engine
    Engine* engine = Engine::getInstance();

    std::string configFilepath = "config/settings.ini";

    if(argc > 1) {
        configFilepath = argv[1];
    }

    // load configuration
    engine->loadConfig(configFilepath);

    // set the update & draw functions
    engine->setUpdateFunction(&update);
    engine->setRenderFunction(&render);

    // create main engine window
    engine->createMainWindow();

    // try to compile a basic shader
    Shader basicShader = engine->createShader("basic.150.vert", "basic.150.frag");
    basicShader.load();

	return EXIT_SUCCESS;
}

void update(Engine& engine) {
    
}

void render(Engine& engine) {

}