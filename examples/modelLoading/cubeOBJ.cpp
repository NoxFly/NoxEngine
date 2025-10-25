//!shared

#include <iostream>
#include <cmath>
#include <chrono>

#include "NoxEngine/engine.hpp"

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
	Scene scene;
	
	PerspectiveCamera camera(
		config.getFloatValue("CAMERA", "fov", 45.0f),
		renderer.getAspect(),
		0.1f,
		config.getFloatValue("CAMERA", "far", 1000.f)
	);

	FPSControls controls(renderer, camera);

	controls.setSensitivity(config.getFloatValue("CAMERA", "sensitivity", 0.1f));
	controls.setSpeed(config.getFloatValue("CAMERA", "speed", 5.0f));
	controls.setFloatingState(true);
	controls.setLerpFactor(2.0f);


	auto meshData = OBJLoader::load("cube.obj");
	auto mesh = Mesh::create(meshData.geometry, meshData.material);

	auto cubeE = Actor::create();
	cubeE->setPosition(0.0f, 0.0f, 0.0f);
	cubeE->addComponent(mesh);
	
	scene.add(cubeE);

	camera.setPosition(0.f, 2.f, 6.f);
	camera.lookAt(0.f, 0.f, 0.f);

	while (!renderer.shouldClose()) {
		controls.update();
		renderer.render(scene, camera);
	}

	return EXIT_SUCCESS;
}