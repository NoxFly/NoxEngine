#include <iostream>

#include "Engine.h"

void update(Engine& engine);
void render(Engine& engine);

int main(int argc, char **argv) {
    Engine* engine = Engine::getInstance();

    std::string configFilepath = "config/settings.ini";

    if(argc > 1) {
        configFilepath = argv[1];
    }

    engine->loadConfig(configFilepath);

    engine->setUpdateFunction(&update);
    engine->setRenderFunction(&render);

    engine->createMainWindow();

	return EXIT_SUCCESS;
}

void update(Engine& engine) {
    
}

void render(Engine& engine) {

}