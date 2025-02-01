//!shared

#include <iostream>
#include <chrono>

#include "core/engine.hpp"
#include "Console.hpp"
#include "IniSet.hpp"

using namespace NoxEngine;

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	IniSet config;

	if (!config.loadFromFile("res/config/config2.ini")) {
		Console::error("main", "Failed to load configuration");
		return EXIT_FAILURE;
	}

	Renderer renderer(config);
	Scene scene;
	PerspectiveCamera camera(45.f, renderer.getAspect(), 0.1f, 1000.f);

	PointerLockControls controls(renderer, camera);

	Texture::load("tile", "dev_tile_orange.png");

	auto addCubes = [&](float x, float y, float z, float xOffset, float yOffset, float zOffset) {
		for (uint i = 0; i < 10; i++) {
			auto cube = std::make_shared<Cube>(1.f, "tile");
			cube->setPosition(x + i * xOffset, y + i * yOffset, z + i * zOffset);
			scene.add(cube);
		}
	};

	addCubes(0.f, 0.f, 1.f, 0.f, 0.f, 1.f);  // Positive Z direction
	addCubes(0.f, 0.f, -1.f, 0.f, 0.f, -1.f); // Negative Z direction
	addCubes(1.f, 0.f, 0.f, 1.f, 0.f, 0.f);  // Positive X direction
	addCubes(-1.f, 0.f, 0.f, -1.f, 0.f, 0.f); // Negative X direction
	addCubes(0.f, 1.f, 0.f, 0.f, 1.f, 0.f);  // Positive Y direction
	addCubes(0.f, -1.f, 0.f, 0.f, -1.f, 0.f); // Negative Y direction


	auto light = std::make_shared<AmbientLight>(Color(255, 255, 255), 25.f);
	light->setPosition(5.f, 2.f, 2.f);

	scene.add(light);

	camera.setPosition(0.f, 2.f, 6.f);
	camera.lookAt(0.f, 0.f, 0.f);

	while (!renderer.shouldClose()) {
		controls.update();
		renderer.render(scene, camera);
	}

	return EXIT_SUCCESS;
}