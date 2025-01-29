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

	Texture::load("tile", "dev_tile_orange.png");


	auto cubeA = std::make_shared<Cube>(1.2f, "tile");
	cubeA->setPosition(-1.f, 0.f, 0.f);

	scene.add(cubeA);

	auto light = std::make_shared<AmbientLight>(Color(255, 255, 255), 25.f);
	light->setPosition(5.f, 2.f, 2.f);

	scene.add(light);

	camera.setPosition(0.f, 2.f, 6.f);
	camera.lookAt(0.f, 0.f, 0.f);

	Uint64 previousTime = SDL_GetPerformanceCounter();
	double totalElapsedTime = 0.0;

	while (!renderer.shouldClose()) {
		renderer.render(scene, camera);

		// get elapsed time since last iteration
		Uint64 now = SDL_GetPerformanceCounter();
		double deltaTime = static_cast<double>(
			(now - previousTime) / static_cast<double>(SDL_GetPerformanceFrequency())
		);
		previousTime = now;
		totalElapsedTime += deltaTime;



		renderer.updateInput();
	}

	return EXIT_SUCCESS;
}