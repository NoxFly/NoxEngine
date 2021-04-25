#ifndef ENGINE_H
#define ENGINE_H

#include "Video.h"
#include "Video_driver.h"
#include "Input.h"
#include "SimpleIni.h"
#include "Shader.h"

#include <mutex>
#include <thread>
#include <signal.h>
#include <map>
#include <functional>

class Engine {
	public:
        static Engine* getInstance();
		Engine(const Engine &);
		~Engine();

        static void SIGINT_handler(int s);

        void terminate();
        void loadConfig(CSimpleIniA &newConfig);
        void loadConfig(const std::string &configFilepath);
        void createMainWindow();
        void setUpdateFunction(std::function<void(Engine& engine)> func);
        void setRenderFunction(std::function<void(Engine& engine)> func);
        Shader createShader(std::string vertexName, std::string fragmentName);

    private:
        Engine();
        void loop();
        void update();
        void updateVideo();
        
        int getWindowConfigX() const;
        int getWindowConfigY() const;
        int getWindowConfigWidth() const;
        int getWindowConfigHeight() const;

        void setResourcesPath(const std::string &path);
        void setShadersPath(const std::string &path);

        Uint32 fps;
        bool _exit_loop;
        std::function<void(Engine& engine)> _updateFunction;
        std::function<void(Engine& engine)> _renderFunction;
        Video video;
        Input video_input;
        std::mutex loop_mutex;
        std::thread thread_loop;
        struct sigaction sigIntHandler;
        std::map<std::string, std::string> paths;
        CSimpleIniA config;
};

#endif // ENGINE_H