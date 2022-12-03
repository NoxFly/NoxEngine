#include <iostream>


#define __NOX_ENGINE_3D__


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


    Scene3D scene;
    PerspectiveCamera camera(glm::radians(70.0f), renderer.getAspect(), 0.1f, 1000.0f);


    // ---------- STEP 2 : assets initialization (shaders, textures, sounds, fonts) ----------

    // load shaders
    Shader::setShadersPath(config.getValue("PATH", "shaders"));
    Shader::setDefaultGLSLversion(renderer.getCompactGLversion());
    Shader::loadFolder();

    // load textures
    Texture::setTexturesPath(config.getValue("PATH", "textures"));
    Texture::load("stone", "stonebrick_cracked.png");



    // ---------- STEP 3 : create entities ----------

    auto cube = std::make_shared<Cube>(1,/*  "stone", */ Color(235, 164, 167));

    cube->setPosition(0, 0, 0);

    // add the entity to the scene
    scene.add(cube);

    // place the camera so it looks the entity
    camera.setPosition(3, 2, 3);
    camera.lookAt(0, 0, 0);


    // ---------- STEP 4 : main loop ----------

    while(!renderer.shouldClose()) {
        renderer.updateInput();
        
        // do app stuff
        renderer.render(&scene, &camera);
    }

    return 0;
}