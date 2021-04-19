#include "Config.h"
#include "SimpleIni.h"

Config getConfigFromFile(std::string filename) {
    Config config;

    CSimpleIniA ini;
    ini.SetUnicode();

    SI_Error rc = ini.LoadFile(filename.c_str());

    if(rc < 0) {
        return config;
    }

    const char* value;

    // app
    if((value = ini.GetValue("app", "name")) != nullptr) {
        config.appName = value;
    }

    // window
    if((value = ini.GetValue("window", "width")) != nullptr) {
        config.windowWidth = atoi(value);
    }

    if((value = ini.GetValue("window", "height")) != nullptr) {
        config.windowHeight = atoi(value);
    }

    if((value = ini.GetValue("window", "fullscreen")) != nullptr) {
        config.isFullscreen = strcmp(value, "true") == 0;
    }

    // game
    if((value = ini.GetValue("engine", "fov")) != nullptr) {
        config.fov = atoi(value);
    }

    if((value = ini.GetValue("engine", "renderDistance")) != nullptr) {
        config.renderDistance = atoi(value);
    }

    if((value = ini.GetValue("engine", "opengl_major_version")) != nullptr) {
        config.opengl_major_version = atoi(value);
    }

    if((value = ini.GetValue("engine", "opengl_minor_version")) != nullptr) {
        config.opengl_minor_version = atoi(value);
    }

    return config;
}