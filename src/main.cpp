#include <iostream>

#include "IniSet.h"
#include "Console.h"
#include "Scene.h"
#include "Renderer.h"
#include "PerspectiveCamera.h"
#include "Shader.h"
#include "Cube.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    IniSet config;

    if(!config.loadFromFile("./config/config.ini")) {
        Console::error("Failed to load the configuration.\nExiting.");
        return 1;
    }


    // Scene scene;
    Renderer renderer(config);
    PerspectiveCamera camera(glm::radians(70.0f), renderer.getAspect(), 0.1f, 100.0f);
    Scene scene;


    // Shader test
    Shader::setShadersPath(config.getValue("PATH", "shaders"));
    Shader::load("color3D", renderer.getCompactGLversion());


    Cube cube(1, "color3D", Color(135, 71, 38));

    scene.add(&cube);

    // camera.setPosition(0, 0, 5);
    // camera.lookAt(0, 0, 0);

    double rx = 0.0;
    double rz = 0.0;


    while(!renderer.shouldClose()) {
        // do app stuff
        renderer.updateInput();

        // o.setRotation(rx, 0, rz);

        rx += 0.1;
        rz += 0.1;

        if(rx >= 360.0) rx -= 360.0;
        if(rz >= 360.0) rz -= 360.0;

        renderer.render(&scene, &camera);
    }

    return 0;
}