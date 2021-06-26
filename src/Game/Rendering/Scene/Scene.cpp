#include "Scene.h"

#include <iostream>
#include <SDL2/SDL_image.h>
#include <mutex>

#include "Console.h"

std::mutex sceneMutex;

Scene::Scene():
    draw(0), 
    m_config(nullptr),
    m_running(false), m_video(),
    m_clearColor{ 0, 0, 0 },
    m_camera(),
    m_mvp(0)
{

}

Scene::Scene(IniSet* config):
    draw(0),
    m_config(config),
    m_running(false), m_video(*config, "WINDOW"),
    m_clearColor{ 0, 0, 0 },
    m_camera(),
    m_mvp(0)
{
    if(!m_video.isInitialized()) {
        Console::error("Scene::constructor", "Failed to init video.");
        exit(3);
    }

    m_clearColor = getColorFromString(config->getValue("WINDOW", "background"));
}

Scene::~Scene() {

}


void Scene::updatePlayerControls(const Input &input) {
    m_camera.update(input);
}


void Scene::updateView() {
    if(m_video.isInitialized() && m_running) {
        render();
    }
}

// clear - draw - swap (refresh)
void Scene::render() {
    m_video.clear(m_clearColor);

    m_camera.lookAt(m_mvp->getView());

    // draw stuff
    if(draw != 0)
        draw();

    // refresh
    m_video.swapWindow();
}

// shows the window
void Scene::show() {
    m_running = true;
    m_video.show();
}


// hides the window
void Scene::close() {
    m_running = false;
    m_video.hide();
}

// returns the opengl version as a string : for example, for version 1.1, returns "11"
GLuint Scene::getCompactGLversion() const {
    GLuint major = m_config->getIntValue("WINDOW", "OPENGL_MAJOR_VERSION", 1);
    GLuint minor = m_config->getIntValue("WINDOW", "OPENGL_MINOR_VERSION", 1);

    return (GLuint)std::stoul(std::to_string(major) + std::to_string(minor));
}


bool Scene::isOpen() const {
    return m_running;
}


void Scene::setDrawFunction(std::function<void()> drawFunction) {
    draw = drawFunction;
}


Camera& Scene::getCamera() {
    return m_camera;
}


Video* Scene::getVideo() {
    return &m_video;
}


void Scene::bindMatrices(MatricesMVP& mvp) {
    m_mvp = &mvp;
}