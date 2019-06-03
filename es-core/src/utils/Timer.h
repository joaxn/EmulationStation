#include <iostream>
#include <thread>
#include <chrono>
#include "Log.h"

class Timer {
    bool clear = false;

    public:
        void setTimeout(const std::function<void()>& func, int delay);
        void setInterval(const std::function<void()>& func, int interval);
        void stop();

};

void Timer::setTimeout(const std::function<void()>& func, int delay) {
    this->clear = false;
    std::thread t([=]{
        if(this->clear) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(this->clear) return;
        func();
    });
    t.detach();
}

void Timer::setInterval(const std::function<void()>& func, int interval) {
    LOG(LogError) << "Interval launched";
	this->clear = false;
    std::thread t([=]{
        LOG(LogError) << "Interval bevore while";
		while(true) {
			LOG(LogError) << "Interval inside while";
            if(this->clear) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(this->clear) return;
			LOG(LogError) << "Interval launch function";
            func();
        }
    });
    t.detach();
}

void Timer::stop() {
    this->clear = true;
}