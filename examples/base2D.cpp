//!shared

#include <iostream>

#define __NOX_ENGINE_2D__

#include <IniSet.hpp>
#include <Console.hpp>
#include <core/engine.hpp>

using namespace NoxEngine;

int main(int argc, char** argv) {
    (void)argc;
	(void)argv;

	IniSet config;

    if (!config.loadFromFile("res/config/config.ini")) {
		Console::error("main", "Failed to load configuration");
		return EXIT_FAILURE;
	}
    
    Renderer renderer(config);
    Scene2D scene;
    Camera2D camera;

    Texture::load("stone", "stonebrick_cracked.png");

    auto rect = std::make_shared<Rectangle>(10.0f, 5.f);

    scene.add(rect);

    camera.setPosition(0.f, 0.f);
    camera.zoomOut(-10.f);

    while(!renderer.shouldClose()) {
        renderer.render(scene, camera);
    }

    return EXIT_SUCCESS;
}