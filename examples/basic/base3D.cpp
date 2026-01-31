#include <iostream>
#include <cmath>
#include <chrono>

#include "NoxEngine/engine.hpp"

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


	auto phongShader = ShaderLoader::load("res/assets/shaders/phong");
	Material::setDefaultShader(phongShader);

	auto stonebrickTex = TextureLoader::load("stone", "res/assets/textures/stonebrick_cracked.png");


	auto cubeGeometry = BoxGeometry::create(1.f, 1.f, 1.f);
	auto cubeMaterial = Material::create<PhongMaterial>();
	cubeMaterial->setDiffuse(Color(200, 100, 100));

	auto cubeMesh = Mesh::create(cubeGeometry, cubeMaterial);


	// color only
	auto cubeA = Actor::create();
	cubeA->setPosition(-3.f, 0.f, 0.f);
	cubeA->addComponent(cubeMesh);

	// texture only
	auto cubeB = Actor::create();
	cubeB->setPosition(-1.f, 0.f, 0.f);
	cubeB->addComponent(cubeMesh);

	// color + texure
	auto cubeC = Actor::create();
	cubeC->setPosition(1.f, 0.f, 0.f);
	cubeC->addComponent(cubeMesh);

	// color + texture + texture opacity
	auto cubeD = Actor::create();
	cubeD->setPosition(3.f, 0.f, 0.f);
	cubeD->addComponent(cubeMesh);

	// color only
	auto cubeE = Actor::create();
	cubeE->setPosition(-4.f, 3.f, -2.f);
	cubeE->addComponent(cubeMesh);
	cubeE->scale(3.0f);

	scene.add(cubeA);
	scene.add(cubeB);
	scene.add(cubeC);
	scene.add(cubeD);
	scene.add(cubeE);


	auto light = Light::create<AmbientLight>(Color(255, 255, 255), 1.f);

	scene.add(light);

	camera.setPosition(0.f, 2.f, 6.f);
	camera.lookAt(0.f, 0.f, 0.f);

	while (!renderer.shouldClose()) {
		controls.update();
		renderer.render(scene, camera);

		auto delta = renderer.getDeltaTime();
		auto totalElapsedTime = renderer.getTotalTimeElapsed();

		// apply rotation to the cubes along the time and not an iteration counter
		// because if performances drops, the cubes will rotate slower, but time
		// continues at the same speed.
		auto rotationX = delta * 10.0f;
		auto rotationY = 0.0f;
		auto rotationZ = delta * 3.0f;

		// apply scaling animation to one of the cubes
		float animationDuration = 1.0f; // in seconds
		float bounceTime = fmod(totalElapsedTime, animationDuration); // Repeat every x seconds
    	float cubeDScaling = 1.0f + easeInOutBack(bounceTime / animationDuration) * 0.5f;

		cubeA->rotate(rotationX, rotationY, rotationZ);
		cubeB->rotate(rotationX, rotationY, rotationZ);
		cubeC->rotate(rotationX, rotationY, rotationZ);
		cubeD->rotate(rotationX, rotationY, rotationZ);
		cubeE->rotate(rotationX * 1.5f, rotationY, rotationZ);

		cubeD->scale(cubeDScaling, cubeDScaling, cubeDScaling);

		if(renderer.getInput()->isKeyPressed(SDL_SCANCODE_LSHIFT)) {
			if(controls.isInterpolationEnabled()) {
				controls.disableInterpolation();
				Console::log("Disabled interpolation");
			}
			else {
				controls.enableInterpolation();
				Console::log("Enabled interpolation");
			}
		}
	}

	return EXIT_SUCCESS;
}