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
	PerspectiveCamera camera(45.0f, renderer.getAspect(), 0.1f, 1000.f);

    FPSControls controls(renderer, camera);

	controls.setSensitivity(config.getFloatValue("CAMERA", "sensitivity", 0.1f));
	controls.setSpeed(config.getFloatValue("CAMERA", "speed", 5.0f));
	controls.setFloatingState(true);
	controls.setLerpFactor(2.0f);

	auto phongShader = ShaderLoader::load("res/assets/shaders/phong");

	Material::setDefaultShader(phongShader);

	// init mesh
	auto cubeGeometry = BoxGeometry::create(1.f, 1.f, 1.f);
	auto cubeMaterial = Material::create();
	cubeMaterial->setDiffuse(Color(255, 91, 56));
    cubeMaterial->setAmbient(Color(50, 20, 10));
    cubeMaterial->setSpecular(Color(255, 255, 255));
    cubeMaterial->setShininess(0.2f);

	auto cubeMesh = Mesh::create(cubeGeometry, cubeMaterial);

	// create actors
	auto cube = Actor::create();
	cube->setPosition(0.0f, -10.0f, 0.0f);
    cube->scale(10.0f);
	cube->addComponent(cubeMesh);
	
	scene.add(cube);

	auto light = Light::create<AmbientLight>(Color(255, 255, 255), 0.5f);

	scene.add(light);

	camera.setPosition(0.f, 2.f, 6.f);
	camera.lookAt(0.f, 0.f, 0.f);

	while (!renderer.shouldClose()) {
		controls.update();
		renderer.render(scene, camera);
	}

	return EXIT_SUCCESS;
}