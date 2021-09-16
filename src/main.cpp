#include <iostream>

#include "IniSet.h"
#include "Console.h"
#include "Scene.h"
#include "Renderer.h"
#include "PerspectiveCamera.h"
#include "Mesh.h"
#include "Geometry.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    IniSet config;

    if(!config.loadFromFile("./config/config.ini")) {
        Console::error("Failed to load the configuration.\nExiting.");
        return 1;
    }


    const int winWidth = config.getIntValue("WINDOW", "width", 640);
    const int winHeight = config.getIntValue("WINDOW", "height", 480);


    // Scene scene;
    Renderer renderer(config);
    PerspectiveCamera camera(70.0f, winWidth / winHeight, 0.1f, 100.0f);
    Scene scene;


    // Shader test
    Shader::setShadersPath(config.getValue("PATH", "shaders"));
    Shader::load("color2D", renderer.getCompactGLversion());
    Shader::load("color3D", renderer.getCompactGLversion());

    // Texture test
    // Texture::setTexturesPath(config.getValue("PATH", "textures"));
    // Texture::load("stonebrick_cracked.png", "stonebrick");

    const float s = .5;

    const std::vector<float> vertices = {
        -s, -s, -s,     s, -s, -s,      s, s, -s,
        -s, -s, -s,     -s, s, -s,      s, s, -s,

        s, -s, s,       s, -s, -s,      s, s, -s,
        s, -s, s,       s, s, s,        s, s, -s,

        -s, -s, s,      s, -s, s,       s, -s, -s,
        -s, -s, s,      -s, -s, -s,     s, -s, -s,

        -s, -s, s,      s, -s, s,       s, s, s,
        -s, -s, s,      -s, s, s,       s, s, s,

        -s, -s, -s,     -s, -s, s,      -s, s, s,
        -s, -s, -s,     -s, s, -s,      -s, s, s,

        -s, s, s,       s, s, s,        s, s, -s,
        -s, s, s,       -s, s, -s,      s, s, -s
    };

    Geometry g(vertices);
    Material m(Shader::get("color2D"));

    Mesh o(g, m);

    scene.add(&o);

    camera.setPosition(0, 0, 20);

    float rx = 0;
    float rz = 0;


    while(!renderer.shouldClose()) {
        // do app stuff
        renderer.updateInput();

        o.setRotation(rx, 0, rz);

        rx += 0.1;
        rz += 0.1;

        renderer.render(scene, camera);
    }

    return 0;
}