#include "Scene.h"

#include <iostream>
#include <SDL2/SDL_image.h>
#include <mutex>

#include "Console.h"

std::mutex sceneMutex;

Scene::Scene():
    config(nullptr),
    fps(0), running(false), video(),
    earlyLoop(0), endLoop(0), spentTime(0),
    clearColor{ 0, 0, 0 },
    draw(0), camera()
{

}

Scene::Scene(IniSet* config):
    config(config),
    fps(0), running(false), video(*config, "WINDOW"),
    earlyLoop(0), endLoop(0), spentTime(0),
    clearColor{ 0, 0, 0 },
    draw(0), camera()
{
    if(!video.isInitialized()) {
        Console::error("Scene::constructor", "Failed to init video.");
        exit(3);
    }

    fps = config->getIntValue("WINDOW", "fps", 30);

    clearColor = getColorFromString(config->getValue("WINDOW", "background"));
}

Scene::~Scene() {

}

void Scene::updatePlayerControls(const Input &input) {
    camera.update(input);
}

void Scene::updateView(const Input &input, glm::mat4& projection, glm::mat4& modelview) {
    if(video.isInitialized()) {
        Uint32 frameRate = 1000 / fps;

        if(running) {
            camera.update(input);

            render(projection, modelview);

            endLoop = SDL_GetTicks();
            spentTime = endLoop - earlyLoop;

            if(spentTime < frameRate)
                SDL_Delay(frameRate - spentTime);
        }
    }
}

void Scene::render(glm::mat4& projection, glm::mat4& modelview) {
    video.clear(clearColor);

    camera.lookAt(modelview);

    // draw stuff
    if(draw != 0)
        draw();

    // refresh
    video.swapWindow();
}

void Scene::show() {
    running = true;
    video.show();
}

void Scene::close() {
    running = false;
    video.hide();
}

GLuint Scene::getCompactGLversion() const {
    GLuint major = config->getIntValue("WINDOW", "OPENGL_MAJOR_VERSION", 1);
    GLuint minor = config->getIntValue("WINDOW", "OPENGL_MINOR_VERSION", 1);

    return (GLuint)std::stoul(std::to_string(major) + std::to_string(minor));
}

bool Scene::isOpen() const {
    return running;
}

void Scene::setDrawFunction(std::function<void()> drawFunction) {
    draw = drawFunction;
}

Camera Scene::getCamera() {
    return camera;
}

void Scene::bindMatrixes(glm::mat4& projection, glm::mat4& modelview, glm::mat4& saveModelview) {
    this->projection = &projection;
    this->modelview = &modelview;
    this->saveModelview = &saveModelview;

    camera.lookAt(modelview);
}

Video* Scene::getVideo() {
    return &video;
}