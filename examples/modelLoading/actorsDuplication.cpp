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

	auto phongShader = ShaderLoader::load("res/assets/shaders/phong");

	Material::setDefaultShader(phongShader);

	// init mesh
	auto cubeGeometry = BoxGeometry::create(1.f, 1.f, 1.f);
	auto cubeMaterial = Material::create();
	cubeMaterial->setDiffuse(Color(200, 100, 100));

	auto cubeMesh = Mesh::create(cubeGeometry, cubeMaterial);

	// create actors
	auto cubeA = Actor::create();
	cubeA->setPosition(-1.0f, 0.0f, 0.0f);
	cubeA->setRotation(25.0f, 0.0f, 25.0f);
	cubeA->addComponent(cubeMesh);

	auto cubeB = Actor::create();
	cubeB->setPosition(1.0f, 0.0f, 0.0f);
	cubeB->setRotation(25.0f, 0.0f, 25.0f);
	cubeB->addComponent(cubeMesh);
	
	// add actors to the scene
	scene.add(cubeA);
	scene.add(cubeB);

	// create light
	auto light = Light::create<AmbientLight>(Color(255, 255, 255), 1.f);

	// add light to the scene
	scene.add(light);

	// setup camera
	camera.setPosition(0.f, 2.f, 6.f);
	camera.lookAt(0.f, 0.f, 0.f);

	// main loop
	while (!renderer.shouldClose()) {
		renderer.render(scene, camera);
	}

	return EXIT_SUCCESS;
}