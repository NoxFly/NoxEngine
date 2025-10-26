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

	auto cubeGeometry = BoxGeometry::create(1.f, 1.f, 1.f);
	auto cubeMaterial = Material::create<PhongMaterial>(Color(100, 200, 255));

	auto mesh = Mesh::create(cubeGeometry, cubeMaterial);

	auto cube = Actor::create()
		->setPosition(0.f, 0.f, 0.f)
		->setRotation(45.f, 25.f, 0.f);

	cube->addComponent(mesh);

	scene.add(cube);

	auto light = std::make_shared<AmbientLight>(Color(255, 255, 255), 25.f);

	scene.add(light);

	camera.setPosition(0.f, 2.f, 6.f);
	camera.lookAt(0.f, 0.f, 0.f);

	while (!renderer.shouldClose()) {
		renderer.render(scene, camera);
	}

	return EXIT_SUCCESS;
}