#include <iostream>

#include "Engine.h"


int main(int argc, char **argv) {
    Engine* engine = Engine::getInstance();

    engine->setResourcesPath("resources/");
    engine->loadConfig("config/settings.ini");

    engine->createMainWindow();

	return EXIT_SUCCESS;
}