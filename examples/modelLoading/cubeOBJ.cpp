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

	ShaderLoader::load("res/assets/shaders/phong");

	auto meshData = OBJLoader::load("res/assets/models/cube.obj");

	meshData.material = Material::create<PhongMaterial>();
	meshData.material->setDiffuse(Color(255, 91, 56));
    meshData.material->setAmbient(Color(50, 20, 10));
    meshData.material->setSpecular(Color(255, 255, 255));
    meshData.material->setShininess(0.2f);

	auto mesh = Mesh::create(meshData.geometry, meshData.material);

	auto cube = Actor::create();
	cube->setPosition(0.0f, 0.0f, 0.0f);
	cube->setRotation(25.0f, 0.0f, 5.0f);
	cube->addComponent(mesh);
	
	scene.add(cube);

	auto light = Light::create<AmbientLight>(Color(255, 255, 255), 1.f);

	scene.add(light);

	camera.setPosition(0.f, 2.f, 6.f);
	camera.lookAt(0.f, 0.f, 0.f);

	while (!renderer.shouldClose()) {
		controls.update();
		renderer.render(scene, camera);
	}

	return EXIT_SUCCESS;
}