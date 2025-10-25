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

	auto cube = Actor::create()
		->setPosition(0.f, 0.f, 0.f)
		->setRotation(45.f, 25.f, 0.f);

	cube->addComponent<Cube>(1.f, Color(150, 50, 10));

	scene.add(cube);

	auto light = std::make_shared<AmbientLight>(Color(255, 255, 255), 25.f);
	light->setPosition(5.f, 2.f, 2.f);

	scene.add(light);

	camera.setPosition(0.f, 2.f, 6.f);
	camera.lookAt(0.f, 0.f, 0.f);

	while (!renderer.shouldClose()) {
		renderer.render(scene, camera);

		// renderer.close();
	}

	Console::log("Camera Orientation: "
		+ std::to_string(camera.getOrientation().w) + ", "
		+ std::to_string(camera.getOrientation().x) + ", "
		+ std::to_string(camera.getOrientation().y) + ", "
		+ std::to_string(camera.getOrientation().z)
	);

	return EXIT_SUCCESS;
}