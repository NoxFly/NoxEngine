#include "Engine.h"

#include <iostream>
#include <sys/stat.h>
#include <vector>

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
        std::cerr << "[Error]   Engine constructor : Failed to create main window" << std::endl;
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
            std::cerr << "[Warning] Engine::loadConfig : Unknown section " << section.pItem << std::endl;
            newConfig.Delete(section.pItem, nullptr);
        }

        else {
            newConfig.GetAllKeys(section.pItem, keys);
            config.GetAllKeys(section.pItem, _keys);

            for(auto key : keys) {
                if(!std::count(_config.at(section.pItem).begin(), _config.at(section.pItem).end(), key.pItem)) {
                    std::cerr << "[Warning] Engine::loadConfig : Unknown key " << key.pItem << std::endl;
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

    std::cout << "[Info]    Engine::loadConfig : New configuration loaded" << std::endl;
}

void Engine::loadConfig(const std::string &configFilepath) {
    std::string ext = configFilepath.substr(configFilepath.find_last_of(".") + 1);

    if(ext != "ini") {
        std::cerr << "[Error]   Engine::loadConfig : Wrong file type. ini expected, " << ext << " given" << std::endl;
        return;
    }

    struct stat buffer;   
    bool exists = stat(configFilepath.c_str(), &buffer) == 0;

    if(!exists) {
        std::cerr << "[Error]   Engine::loadConfig : File not found" << std::endl;
        return;
    }

    if(!(buffer.st_mode & S_IFREG)) {
        std::cerr << "[Error]   Engine::loadConfig : Not a file" << std::endl;
        return;
    }


    CSimpleIniA ini;
    ini.SetUnicode();
    SI_Error rc = ini.LoadFile(configFilepath.c_str());

    if(rc < 0) {
        std::cerr << "[Error]   Engine::loadConfig : config file not found, or unreadable" << std::endl;
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
            std::cerr << "[Error]   Engine::setResourcesPath : Given path must be a folder" << std::endl;
        }
    }

    else {
        std::cerr << "[Error]   Engine::setResourcesPath : Could not locate resources folder" << std::endl;
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
            std::cerr << "[Error]   Engine::setShadersPath : Given path must be a folder" << std::endl;
        }
    }

    else {
        std::cerr << "[Error]   Engine::setShadersPath : Could not locate shaders folder" << std::endl;
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

void Engine::setUpdateFunction(std::function<void(Engine& engine)> func) {
    _updateFunction = func;
}

void Engine::setRenderFunction(std::function<void(Engine& engine)> func) {
    _renderFunction = func;
}

Shader Engine::createShader(std::string vertexName, std::string fragmentName) {
    return Shader(paths["shaders"] + vertexName, paths["shaders"] +fragmentName);
}