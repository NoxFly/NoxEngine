#include <iostream>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <filesystem>

#ifdef __MINGW32__
#include <windows.h>
#endif

#include "IniSet.h"
#include "Console.h"
#include "Application.h"


int run(int argc, char** argv, std::string os) {
    IniSet ini;
    std::string argv0 = argv[0];

    size_t posLastDir = argv0.find_last_of('/');

    if(posLastDir == string::npos)
        posLastDir = argv0.find_last_of('\\');

    std::string exePath = argv0.substr(0, posLastDir);
    
    if(!ini.loadFromFile(exePath + "/config/config.ini")) {
        Console::error("Failed to load the configuration.\nExiting.");
        return 1;
    }

    Application app(ini, exePath);

    app.start();
    
    return 0;
}

int main(int argc, char** argv) {
    return run(argc, argv, "unix");
}