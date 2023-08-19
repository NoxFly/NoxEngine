#include <iostream>

#define __NOX_ENGINE_3D__

#include "core/engine.hpp"
#include "Console.hpp"
#include "IniSet.hpp"


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



	auto cube = std::make_shared<Cube>(1.f, "stone", Color(150, 50, 10));
	cube->setPosition(0.f, 0.f, 0.f);

	scene.add(cube);


	auto light = std::make_shared<AmbientLight>(Color(255, 255, 255), 50.f);
	light->setPosition(5.f, 2.f, 2.f);

	scene.add(light);


	camera.setPosition(3.f, 2.f, 3.f);
	camera.lookAt(0.f, 0.f, 0.f);



	while (!renderer.shouldClose()) {
		renderer.render(scene, camera);

		cube->rotate(0.1f, 0.f, 0.05f);

		renderer.updateInput();
	}


	return EXIT_SUCCESS;
}