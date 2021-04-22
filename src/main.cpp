#include <iostream>
#include "SimpleIni.h"

#include "Engine.h"


int main(int argc, char **argv) {
    Engine* engine = Engine::getInstance();

    // engine setup
    //engine->setPath("config", "./config");

    //engine->loadConfigFromFile("settings.ini");

    // init opengGL
    // if(!engine->loadContext()) {
    //     return EXIT_FAILURE;
    // }

    //std::cout << "Initialized Engine with OpenGL version " << engine->getGLVersion() << std::endl;

    //
    // create window
    //engine->createWindow();
    // engine->terminateOnWindowClose();
    //std::cout << "create window" << std::endl;

	return EXIT_SUCCESS;
}