#include <iostream>

#define __NOX_ENGINE_2D__

#include <IniSet.hpp>
#include <Console.hpp>
#include <core/engine.hpp>

using namespace NoxEngine;

int main(int argc, char** argv) {
    std::string configPath = "_resources/config/config.ini";

    if(argc > 1) {
        configPath = argv[1];
    }
    
    IniSet config;

    if(!config.loadFromFile(configPath)) {
        Console::error("Failed to load the configuration.\nExiting.");
        return 1;
    }

    Renderer renderer(config);

    Scene2D scene;
    Camera2D camera;

    Texture::load("stone", "stonebrick_cracked.png");

    auto rect = std::make_shared<Rectangle>();

    scene.add(rect);

    while(!renderer.shouldClose()) {
        renderer.render(scene, camera);
        renderer.updateInput();
    }

    return 0;
}