#include "Engine.h"

#include <iostream>
#include <sys/stat.h>
#include <vector>

Engine* Engine::getInstance() {
    static Engine engine;
    return &engine;
}

Engine::Engine():
    video(),
    video_input(),
    _exit_loop(false),
    thread_loop(&Engine::loop, this),
    paths{}
{
    // handle ctrl^C
    sigIntHandler.sa_handler = Engine::SIGINT_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // load default config
    config.LoadData(
        "[app]\nname=OpenGL + SDL Window\n"
        "[window]\nx=center\ny=center\nwidth=640\nheight=480"
    );

    // initialize opengl
    Video_driver::loadLastGlVersion();
}

Engine::Engine(const Engine &) {
    
}

Engine::~Engine() {
    thread_loop.join();
}

void Engine::SIGINT_handler(int s) {
    Engine::getInstance()->terminate();
}

void Engine::createMainWindow() {
    if(video.hasWindow()) {
        std::cerr << "[Warning] Engine::createMainWindow : Main window already created" << std::endl;
        return;
    }

    SDL_Window* w = video.createWindow(
        getWindowConfigX(),
        getWindowConfigY(),
        getWindowConfigWidth(),
        getWindowConfigHeight(),
        config.GetValue("app", "name")
    );
    
    // create main window
    if(w == nullptr) {
        std::cerr << "[Error] Engine constructor : Failed to create main window" << std::endl;
    }
}

void Engine::loop() {
    // wait for video initialization
    while(!_exit_loop && !Video_driver::is_initialized);

    // main loop
    while(!_exit_loop) {
        update();
    }
}

void Engine::update() {
    if(video.hasWindow()) {
        updateVideo();
    }
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

void Engine::loadConfig(CSimpleIniA &newConfig) {
    std::lock_guard<std::mutex> lock(loop_mutex);

    const std::map<std::string, std::vector<std::string>> model_sections = {
        { "app", { "name" } },
        { "window", { "x", "y", "width", "height" } }
    };

    // verify input newConfig : clean unknown sections / keys
    CSimpleIniA::TNamesDepend sections, keys;
    newConfig.GetAllSections(sections);

    for(auto section : sections) {
        if(!model_sections.count(section.pItem)) {
            std::cerr << "[Warning] Engine::loadConfig : Unknown section " << section.pItem << std::endl;
            newConfig.Delete(section.pItem, nullptr);
        }

        else {
            newConfig.GetAllKeys(section.pItem, keys);
            std::vector<std::string> v = model_sections.at(section.pItem);

            for(auto key : keys) {
                if(!std::count(v.begin(), v.end(), key.pItem)) {
                    std::cerr << "[Warning] Engine::loadConfig : Unknown key " << key.pItem << std::endl;
                    newConfig.Delete(section.pItem, key.pItem);
                }
            }
        }
    }

    CSimpleIniA::TNamesDepend _sections, _keys;

    newConfig.GetAllSections(sections);
    config.GetAllSections(_sections);

    std::vector<std::string> __sections;

    for(auto s : _sections)
        __sections.push_back(s.pItem);

    // update engine config
    for(auto section : sections) {
        const char* sctName = section.pItem;

        if(!std::count(__sections.begin(), __sections.end(), sctName)) {
            config.SetValue(sctName, nullptr, nullptr);
        }

        newConfig.GetAllKeys(sctName, keys);

        for(auto key : keys) {
            const char* keyName = key.pItem;
            config.SetValue(sctName, keyName, newConfig.GetValue(sctName, keyName));
        }
    }

    std::cout << "[Info] Engine::loadConfig : New configuration loaded" << std::endl;
}

void Engine::loadConfig(const std::string &configFilepath) {
    std::string ext = configFilepath.substr(configFilepath.find_last_of(".") + 1);

    if(ext != "ini") {
        std::cerr << "[Error] Engine::loadConfig : Wrong file type. ini expected, " << ext << " given" << std::endl;
        return;
    }

    struct stat buffer;   
    bool exists = stat(configFilepath.c_str(), &buffer) == 0;

    if(!exists) {
        std::cerr << "[Error] Engine::loadConfig : File not found" << std::endl;
        return;
    }

    if(!(buffer.st_mode & S_IFREG)) {
        std::cerr << "[Error] Engine::loadConfig : Not a file" << std::endl;
        return;
    }


    CSimpleIniA ini;
    ini.SetUnicode();
    SI_Error rc = ini.LoadFile(configFilepath.c_str());

    if(rc < 0) {
        std::cerr << "[Error] Engine::loadConfig : config file not found, or unreadable" << std::endl;
        return;
    }

    loadConfig(ini);
}

void Engine::setResourcesPath(const std::string &path) {
    struct stat buffer;   
    bool exists = stat(path.c_str(), &buffer) == 0;

    if(exists) {
        if(buffer.st_mode & S_IFDIR) {
            paths["resources"] = path;
        }

        else {
            std::cerr << "[Error] Engine::setResourcesPath : Given path must be a folder" << std::endl;
        }
    }

    else {
        std::cerr << "[Error] Engine::setResourcesPath : Could not locate resources folder" << std::endl;
    }
}

int Engine::getWindowConfigX() const {
    std::string v = config.GetValue("window", "x");

    if(!v.compare("center"))
        return SDL_WINDOWPOS_CENTERED;

    try {
        return std::stoi(v);
    } catch(std::exception &e) {
        return 0;
    }
}

int Engine::getWindowConfigY() const {
    std::string v = config.GetValue("window", "y");

    if(!v.compare("center"))
        return SDL_WINDOWPOS_CENTERED;

    try {
        return std::stoi(v);
    } catch(std::exception &e) {
        return 0;
    }
}

int Engine::getWindowConfigWidth() const {
    try {
        return std::stoi((std::string)config.GetValue("window", "width"));
    } catch(std::exception &e) {
        return 0;
    }
}

int Engine::getWindowConfigHeight() const {
    try {
        return std::stoi((std::string)config.GetValue("window", "height"));
    } catch(std::exception &e) {
        return 0;
    }
}