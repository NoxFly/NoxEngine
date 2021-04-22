#ifndef ENGINE_H
#define ENGINE_H

#include "Video.h"
#include "Video_driver.h"
#include "Input.h"

#include <mutex>
#include <thread>
#include <signal.h>

class Engine {
	public:
        static Engine* getInstance();
		Engine(const Engine &);
		~Engine();

        static void SIGINT_handler(int s);

    private:
        Engine();
        void loop();
        void loop_process();
        void update();
        void updateVideo();

        Video video;
        Input video_input;
        std::mutex loop_mutex;
        bool _exit_loop;
        std::thread thread_loop;
        struct sigaction sigIntHandler;
};

#endif // ENGINE_H