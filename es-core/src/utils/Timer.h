#include <iostream>
#include <thread>
#include <chrono>
#include "Log.h"

class Timer {
    bool timeClear = false;

    public:
        void setTimeout(const std::function<void()>& func, int delay);
        void setInterval(const std::function<void()>& func, int interval);
        void stop();

};

void Timer::setTimeout(const std::function<void()>& func, int delay) {
    this->timeClear = false;
    std::thread t([=]{
        if(this->timeClear) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(this->timeClear) return;
        func();
    });
    t.detach();
}

void Timer::setInterval(const std::function<void()>& func, int interval) {
    LOG(LogError) << "Interval launched";
	this->timeClear = false;
    std::thread t([=]{
        LOG(LogError) << "Interval bevore while";
		while(true) {
			LOG(LogError) << "Interval inside while";
            if(this->timeClear) return;
			LOG(LogError) << "Interval after timeclear check";
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
			LOG(LogError) << "Interval launch function";
            func();
        }
    });
    t.detach();
}

void Timer::stop() {
    this->timeClear = true;
}