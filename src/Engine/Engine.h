#ifndef ENGINE_H
#define ENGINE_H

#include "Input.h"
#include "Config.h"

#include <map>
#include <string>
#include <thread>
#include <SDL2/SDL.h>

class Engine {
	public:
        static Engine* getInstance();
		Engine(const Engine &);
        ~Engine();

        static void SIGINT_handler(int s);

        void setPath(std::string name, std::string path);
        void loadConfigFromFile(std::string filename);
        bool loadContext();
        bool createWindow();
        void terminate();
        void terminateOnWindowClose();
        std::string getGLVersion() const;

    protected:
        Config m_config;
        Input m_input;
        SDL_Window* m_window;
		SDL_GLContext m_oglContext;

        std::map<std::string, std::string> m_paths;

        bool m_firstInitialization;
        bool m_running;
        bool m_stopOnWindowClose;
        std::thread m_thread_loop;

        int initSDL();
        int initGL();
        void destroyWindow(bool sdlQuit=false);
        void loop();
        std::array<int, 2> getSupportedGLVersion();

    private:
		Engine();
        static Engine* instance;
};

#endif // ENGINE_H