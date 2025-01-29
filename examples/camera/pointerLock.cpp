//!shared

#include <iostream>
#include <chrono>

#define __NOX_ENGINE_3D__

#include "core/engine.hpp"
#include "Console.hpp"
#include "IniSet.hpp"

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
	Scene3D scene;
	PerspectiveCamera camera(45.f, renderer.getAspect(), 0.1f, 1000.f);

	auto control = std::make_shared<PointerLockControl>();

	camera.setControl(control);

	Texture::load("tile", "dev_tile_orange.png");

	for(uint i=0; i < 10; i++) {
		auto cubeA = std::make_shared<Cube>(1.2f, "tile");
		cubeA->setPosition(-1.f, 0.f, (float)i);
		scene.add(cubeA);
	}


	auto light = std::make_shared<AmbientLight>(Color(255, 255, 255), 25.f);
	light->setPosition(5.f, 2.f, 2.f);

	scene.add(light);

	camera.setPosition(0.f, 2.f, 6.f);
	camera.lookAt(0.f, 0.f, 0.f);

	while (!renderer.shouldClose()) {
		renderer.render(scene, camera);
		renderer.updateInput();
	}

	return EXIT_SUCCESS;
}