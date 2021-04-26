#include "Engine.h"

#include <iostream>
#include <sys/stat.h>
#include <vector>

#include "Console.h"

Engine* Engine::getInstance() {
    static Engine engine;
    return &engine;
}

Engine::Engine():
    fps(50),
    video(),
    video_input(),
    _exit_loop(false),
    thread_loop(&Engine::loop, this),
    paths{},
    _updateFunction(nullptr),
    _renderFunction(nullptr)
{
    // handle ctrl^C
    sigIntHandler.sa_handler = Engine::SIGINT_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // load default config
    config.LoadData(
        "[app]\nname=OpenGL + SDL Window\nfps=50\nresourcesFolder=./\nshadersFolder=./\n"
        "[window]\nx=center\ny=center\nwidth=640\nheight=480"
    );

    fps = std::stoi(config.GetValue("app", "fps"));
    setResourcesPath(config.GetValue("app", "resourcesFolder"));
    setShadersPath(config.GetValue("app", "shadersFolder"));

    // initialize opengl
    Video_driver::loadLastGlVersion();

    Shader::setGLSLVersion(Video_driver::opengl_major_version, Video_driver::opengl_minor_version);
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
        Console::warn("Engine::createMainWindow", "Main window already created");
        return;
    }

    if(!video.createWindow(
        getWindowConfigX(),
        getWindowConfigY(),
        getWindowConfigWidth(),
        getWindowConfigHeight(),
        config.GetValue("app", "name")
    )) {
        Console::error("Engine constructor", "Failed to create main window");
    }
}

void Engine::loop() {
    // wait for video initialization
    while(!_exit_loop && !Video_driver::is_initialized);

    // main loop
    while(!_exit_loop) {
        video.updateTick(fps);

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
        return;
    }

    if(_updateFunction != nullptr)
        _updateFunction(*this);

    video.clear();

    if(_renderFunction != nullptr) {
        _renderFunction(*this);
    }

    video.display();
}

void Engine::terminate() {
    _exit_loop = true;
}

void Engine::loadConfig(CSimpleIniA &newConfig) {
    std::lock_guard<std::mutex> lock(loop_mutex);

    // verify input newConfig : clean unknown sections / keys
    CSimpleIniA::TNamesDepend sections, keys, _sections, _keys;

    newConfig.GetAllSections(sections);
    config.GetAllSections(_sections);

    // recover every sections and keys from current config as map<vector<string>>
    std::map<std::string, std::vector<std::string>> _config;

    for(auto section : _sections) {
        _config[section.pItem] = {};

        config.GetAllKeys(section.pItem, _keys);

        for(auto key : _keys) {
            _config[section.pItem].push_back(key.pItem);
        }
    }


    for(auto section : sections) {
        if(!_config.count(section.pItem)) {
            Console::warn("Engine::loadConfig", "Unknown section " + (std::string)section.pItem);
            newConfig.Delete(section.pItem, nullptr);
        }

        else {
            newConfig.GetAllKeys(section.pItem, keys);
            config.GetAllKeys(section.pItem, _keys);

            for(auto key : keys) {
                if(!std::count(_config.at(section.pItem).begin(), _config.at(section.pItem).end(), key.pItem)) {
                    Console::warn("Engine::loadConfig", "Unknown key " + (std::string)key.pItem);
                    newConfig.Delete(section.pItem, key.pItem);
                }
            }
        }
    }

    newConfig.GetAllSections(sections);

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

    fps = std::stoi(config.GetValue("app", "fps"));
    config.Delete("app", "fps");

    setResourcesPath(config.GetValue("app", "resourcesFolder"));
    config.Delete("app", "resourcesFolder");

    setShadersPath(config.GetValue("app", "shadersFolder"));
    config.Delete("app", "shadersFolder");

    Console::info("Engine::loadConfig", "New configuration loaded");
}

void Engine::loadConfig(const std::string &configFilepath) {
    std::string ext = configFilepath.substr(configFilepath.find_last_of(".") + 1);

    if(ext != "ini") {
        Console::error("Engine::loadConfig", "Wrong file type. ini expected, " + ext + " given");
        return;
    }

    struct stat buffer;   
    bool exists = stat(configFilepath.c_str(), &buffer) == 0;

    if(!exists) {
        Console::error("Engine::loadConfig", "File not found");
        return;
    }

    if(!(buffer.st_mode & S_IFREG)) {
        Console::error("Engine::loadConfig", "Not a file");
        return;
    }


    CSimpleIniA ini;
    ini.SetUnicode();
    SI_Error rc = ini.LoadFile(configFilepath.c_str());

    if(rc < 0) {
        Console::error("Engine::loadConfig", "config file not found, or unreadable");
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
            Console::error("Engine::setResourcesPath", "Given path must be a folder");
        }
    }

    else {
        Console::error("Engine::setResourcesPath", "Could not locate resources folder");
    }
}

void Engine::setShadersPath(const std::string &path) {
    struct stat buffer;   
    bool exists = stat(path.c_str(), &buffer) == 0;

    if(exists) {
        if(buffer.st_mode & S_IFDIR) {
            paths["shaders"] = path;
            Shader::setGLSLVersion(Video_driver::opengl_major_version, Video_driver::opengl_minor_version);
        }

        else {
            Console::error("Engine::setShadersPath", "Given path must be a folder");
        }
    }

    else {
        Console::error("Engine::setShadersPath", "Could not locate shaders folder");
    }
}

int Engine::getWindowConfigX() const {
    std::string v = config.GetValue("window", "x");

    if(!v.compare("center"))
        return VIDEO_POS_CENTER;

    try {
        return std::stoi(v);
    } catch(std::exception &e) {
        return 0;
    }
}

int Engine::getWindowConfigY() const {
    std::string v = config.GetValue("window", "y");

    if(!v.compare("center"))
        return VIDEO_POS_CENTER;

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

void Engine::setUpdateFunction(std::function<void(Engine& engine)> func) {
    _updateFunction = func;
}

void Engine::setRenderFunction(std::function<void(Engine& engine)> func) {
    _renderFunction = func;
}

Shader Engine::createShader(std::string vertexName, std::string fragmentName) {
    return Shader(paths["shaders"] + vertexName, paths["shaders"] +fragmentName);
}