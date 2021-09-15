#include <iostream>

#include "IniSet.h"
#include "Console.h"
#include "Scene.h"
#include "Renderer.h"
#include "PerspectiveCamera.h"
#include "utils.h"
#include "Object.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    IniSet ini;

    if(!ini.loadFromFile("./config/config.ini")) {
        Console::error("Failed to load the configuration.\nExiting.");
        return 1;
    }


    const int winWidth = ini.getIntValue("WINDOW", "width", 640);
    const int winHeight = ini.getIntValue("WINDOW", "height", 480);


    // Scene scene;
    Renderer renderer(ini);
    PerspectiveCamera camera(70.0f, winWidth / winHeight, 0.1f, 100.0f);
    Scene scene;

    while(!renderer.shouldClose()) {
        // do app stuff
        renderer.updateInput();

        renderer.render(scene, camera);
    }

    return 0;
}