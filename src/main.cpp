#ifdef MEMORYCHECK
#include "LeakDetector.h"
#endif

#include <iostream>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <filesystem>

#include "IniSet.h"
#include "Console.h"
#include "Application.h"


int main(int argc, char** argv) {
    // get executable absolute path
    std::string argv0 = argv[0];
    (void)argc;

    size_t posLastDir = argv0.find_last_of('/');

    if(posLastDir == std::string::npos)
        posLastDir = argv0.find_last_of('\\');

    std::string exePath = argv0.substr(0, posLastDir);
    //

    // load application's config - REQUIRED
    IniSet ini;

    if(!ini.loadFromFile(exePath + "/config/config.ini")) {
        Console::error("Failed to load the configuration.\nExiting.");
        return 1;
    }

    Application app(ini, exePath);

    app.start();
    
    return 0;
}