#include <iostream>

// #define DEBUG

#include <IniSet.hpp>
#include <Console.hpp>
#include <core/engine.hpp>

using namespace NoxEngine;


int main(int argc, char** argv) {
    std::string configPath = "./config/config.ini";



    // ---------- OPTIONAL STEP 0 : load configuration ----------

    if(argc > 1) {
        configPath = argv[1];
    }
    
    // config initialization
    IniSet config;

    if(!config.loadFromFile(configPath)) {
        Console::error("Failed to load the configuration.\nExiting.");
        return 1;
    }



    // ---------- STEP 1 : engine initialization ----------

    // /!\ the renderer has to be initialized first to load the OpenGL context.
    Renderer renderer(config);


#if defined(_2D)

    Scene2D scene;
    Camera2D camera;

#elif defined(_3D)

    Scene3D scene;
    PerspectiveCamera camera(glm::radians(70.0f), renderer.getAspect(), 0.1f, 1000.0f);

#endif


    // ---------- STEP 2 : assets initialization (shaders, textures, sounds, fonts) ----------

    // load shaders
    Shader::setShadersPath(config.getValue("PATH", "shaders"));
    Shader::setDefaultGLSLversion(renderer.getCompactGLversion());
    Shader::loadFolder();

    // load textures
    Texture::setTexturesPath(config.getValue("PATH", "textures"));
    Texture::load("stone", "stonebrick_cracked.png");



    // ---------- STEP 3 : create entities ----------

#if defined(_2D)

    auto rect = std::make_shared<Rectangle>();

    // add the entity to the scene
    scene.add(rect);

#elif defined(_3D)

    auto cube = std::make_shared<Cube>(1,/*  "stone", */ Color(235, 164, 167));

    cube->setPosition(0, 0, 0);

    // add the entity to the scene
    scene.add(cube);

    // place the camera so it looks the entity
    camera.setPosition(3, 2, 3);
    camera.lookAt(0, 0, 0);

#endif



    // ---------- STEP 4 : main loop ----------

    while(!renderer.shouldClose()) {
        renderer.updateInput();
        
        // do app stuff
#if defined(_2D) || defined(_3D)
        renderer.render(&scene, &camera);
#endif
    }

    return 0;
}