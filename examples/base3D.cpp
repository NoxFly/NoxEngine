//!shared

#include <iostream>
#include <cmath>
#include <chrono>

#define __NOX_ENGINE_3D__

#include "core/engine.hpp"
#include "Console.hpp"
#include "IniSet.hpp"

using namespace NoxEngine;

float easeInOutBack(float t, float s = 1.70158f) {
	if(t > 0.5f)
		t = 1.0f - t;

	t *= 4.0f;

	if (t < 1.0f) {
        return 0.5f * (t * t * ((s + 1.0f) * t - s));
    } else {
        t -= 2.0f;
        return 0.5f * (t * t * ((s + 1.0f) * t + s) + 2.0f);
    }
}



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

		// apply rotation to the cubes along the time and not an iteration counter
		// because if performances drops, the cubes will rotate slower, but time
		// continues at the same speed.
		auto rotationX = deltaTime * 10.0f;
		auto rotationY = 0;
		auto rotationZ = deltaTime * 3.0f;

		// apply scaling animation to one of the cubes
		float animationDuration = 1.0f; // in seconds
		float bounceTime = fmod(totalElapsedTime, animationDuration); // Repeat every x seconds
    	float cubeDScaling = 1.0f + easeInOutBack(bounceTime / animationDuration) * 0.5f;

		cubeA->rotate(rotationX, rotationY, rotationZ);
		cubeB->rotate(rotationX, rotationY, rotationZ);
		cubeC->rotate(rotationX, rotationY, rotationZ);
		cubeD->rotate(rotationX, rotationY, rotationZ);

		cubeD->scale(cubeDScaling, cubeDScaling, cubeDScaling);

		renderer.updateInput();
	}

	return EXIT_SUCCESS;
}