#include "Engine.h"

#include <iostream>

Engine* Engine::getInstance() {
    static Engine engine;
    return &engine;
}

Engine::Engine():
    video(),
    video_input(),
    _exit_loop(false),
    thread_loop(&Engine::loop, this)
{
    sigIntHandler.sa_handler = Engine::SIGINT_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    Video_driver::loadLastGlVersion();

    if(video.createWindow(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, "OpenGL C++ Engine") == nullptr) {
        std::cout << "Engine constructor Error : Failed to create main window" << std::endl;
        return;
    }
}

Engine::Engine(const Engine &) {
    
}

Engine::~Engine() {
    thread_loop.join();
}

void Engine::SIGINT_handler(int s) {
    Engine::getInstance()->terminate();
}

void Engine::loop() {
    while(!_exit_loop) {
        update();
    }
}

void Engine::update() {
    updateVideo();
}

void Engine::updateVideo() {
    video_input.updateEvents();

    if(video_input.shouldClose(video.id())) {
        video.close();
        _exit_loop = true;
    }
}

void Engine::terminate() {
    _exit_loop = true;
}