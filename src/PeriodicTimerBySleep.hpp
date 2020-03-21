#pragma once

#include <thread>

#include "constant.hpp"
#include "PeriodicTimer.hpp"


template <class T>
class PeriodicTimerBySleep: public PeriodicTimer<T> {
    private:
        void setNextTriggerTime(const TIME& input, 
            TIME& nextTriggerTime, const std::chrono::nanoseconds& wait);

    public:
        PeriodicTimerBySleep<T>(T& t): PeriodicTimer<T>(t){ 
            std::cout << "PeriodicTimerBySleep created" << std::endl;
        }
        void run(std::chrono::nanoseconds wait, unsigned long nr);
};

template <class T>
void PeriodicTimerBySleep<T>::setNextTriggerTime(const TIME& input, 
    TIME& nextTriggerTime, const std::chrono::nanoseconds& wait) {
            nextTriggerTime = input + wait;
    } 

template <class T>    
void PeriodicTimerBySleep<T>::run(std::chrono::nanoseconds wait, unsigned long nr) {

    std::cout << "Starting the timer" << std::endl;   
    unsigned long runs=0;   
    bool ok = true;
    auto timenow = std::chrono::system_clock::now();
    TIME nextSendTime;

    while (ok && runs++ < nr) {        
        setNextTriggerTime(timenow, nextSendTime, wait);
        std::this_thread::sleep_until(nextSendTime);
        timenow = std::chrono::system_clock::now();
        ok = this->task.run(nextSendTime, timenow);
    } 
    std::cout << "Timer finished" << std::endl;
}
