//!shared

#include <iostream>

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

	Texture::load("stone", "stonebrick_cracked.png");


	// color only
	auto cubeA = std::make_shared<Cube>(1.f, Color(150, 50, 10));
	cubeA->setPosition(-3.f, 0.f, 0.f);

	// texture only
	auto cubeB = std::make_shared<Cube>(1.2f, "stone");
	cubeB->setPosition(-1.f, 0.f, 0.f);

	// color + texure
	auto cubeC = std::make_shared<Cube>(1.3f, "stone", Color(150, 50, 10));
	cubeC->setPosition(1.f, 0.f, 0.f);

	// color + texture + texture opacity
	auto cubeD = std::make_shared<Cube>(.5f, "stone", Color(150, 50, 10));
	cubeD->setPosition(3.f, 0.f, 0.f);
	cubeD->getMaterial()->setTextureOpacity(0.5f);

	scene.add(cubeA);
	scene.add(cubeB);
	scene.add(cubeC);
	scene.add(cubeD);

	// auto cube = std::make_shared<Mesh3D>();
	// cube->loadFromFile("cube.obj");
	
	// scene.add(cube);


	auto light = std::make_shared<AmbientLight>(Color(255, 255, 255), 25.f);
	light->setPosition(5.f, 2.f, 2.f);

	scene.add(light);

	camera.setPosition(0.f, 2.f, 6.f);
	camera.lookAt(0.f, 0.f, 0.f);

	while (!renderer.shouldClose()) {
		renderer.render(scene, camera);

		cubeA->rotate(0.1f, 0.f, 0.05f);
		cubeB->rotate(0.1f, 0.f, 0.05f);
		cubeC->rotate(0.1f, 0.f, 0.05f);
		cubeD->rotate(0.1f, 0.f, 0.05f);

		renderer.updateInput();
	}

	return EXIT_SUCCESS;
}