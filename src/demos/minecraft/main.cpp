#include <iostream>

#define __NOX_ENGINE_3D__

#include "core/engine.hpp"
#include "Console/Console.hpp"
#include "IniSet/IniSet.hpp"


using namespace NoxEngine;



int main(int argc, char** argv) {
	IniSet config;

	if (!config.loadFromFile("config/config.ini")) {
		Console::error("main", "Failed to load configuration");
		return EXIT_FAILURE;
	}

	Renderer renderer(config);
	Scene3D scene;
	PerspectiveCamera camera(45.f, renderer.getAspect(), 0.1f, 1000.f);

	Texture::load("stone", "stonebrick_cracked.png");

	auto cube = std::make_shared<Cube>(1,/*  "stone", */ Color(235, 164, 167));

	cube->setPosition(0, 0, 0);

	scene.add(cube);

	camera.setPosition(3, 2, 3);
	camera.lookAt(0, 0, 0);

	while (!renderer.shouldClose()) {
		renderer.render(scene, camera);

		cube->rotate(0.1, 0, 0.05);

		renderer.updateInput();
	}


	return EXIT_SUCCESS;
}