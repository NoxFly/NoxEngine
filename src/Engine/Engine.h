#ifndef ENGINE_H
#define ENGINE_H

#include "Video.h"
#include "Video_driver.h"
#include "Input.h"
#include "SimpleIni.h"

#include <mutex>
#include <thread>
#include <signal.h>
#include <map>

class Engine {
	public:
        static Engine* getInstance();
		Engine(const Engine &);
		~Engine();

        static void SIGINT_handler(int s);

        void terminate();
        void loadConfig(CSimpleIniA &newConfig);
        void loadConfig(const std::string &configFilepath);
        void setResourcesPath(const std::string &path);
        void createMainWindow();

    private:
        Engine();
        void loop();
        void update();
        void updateVideo();
        
        int getWindowConfigX() const;
        int getWindowConfigY() const;
        int getWindowConfigWidth() const;
        int getWindowConfigHeight() const;

        Video video;
        Input video_input;
        std::mutex loop_mutex;
        bool _exit_loop;
        std::thread thread_loop;
        struct sigaction sigIntHandler;
        std::map<std::string, std::string> paths;
        CSimpleIniA config;
};

#endif // ENGINE_H