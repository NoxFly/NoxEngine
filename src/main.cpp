#include <iostream>

#include "IniSet.hpp"
#include "Console.hpp"
#include "Scene.hpp"
#include "Renderer.hpp"
#include "PerspectiveCamera.hpp"
#include "Shader.hpp"
#include "Cube.hpp"

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


    Cube cube(50, Color(150, 150, 150));

    scene.add(&cube);

    renderer.setMouseFocus(true);

    camera.setPosition(0, 0, 5);
    camera.lookAt(0, 0, 0);


    while(!renderer.shouldClose()) {
        // do app stuff
        renderer.updateInput();
        

        // update
        if(renderer.getInput()->isKeyDown(SDL_SCANCODE_ESCAPE)) {
            if(renderer.isMouseFocused())
                renderer.setMouseFocus(false);
        }

        // if mouse isn't captured and a click is performed on the window,
        // capture it back
        if(renderer.getInput()->isMouseButtonDown(SDL_BUTTON_LEFT)) {
            if(!renderer.isMouseFocused())
                renderer.setMouseFocus(true);
        }
        //

        renderer.render(&scene, &camera);
    }

    return 0;
}