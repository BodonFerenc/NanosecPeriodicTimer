#pragma once

#include "constant.hpp"
#include "PeriodicTimer.hpp"

struct Strict {
    static void setNextTriggerTime(const TIME& input, 
            TIME& nextTriggerTime, const std::chrono::nanoseconds& wait) {
            nextTriggerTime += wait;
    }
};

struct JumpForward {
    static void setNextTriggerTime(const TIME& input, 
            TIME& nextTriggerTime, const std::chrono::nanoseconds& wait) {
            nextTriggerTime = input + wait -
                std::chrono::nanoseconds(DURNANO((input + wait).time_since_epoch()) % wait.count());
    }
};


template <class T, class P>
class PeriodicTimerByTimeCheck: public PeriodicTimer<T> {
    public:
        PeriodicTimerByTimeCheck<T, P>(T& t): PeriodicTimer<T>(t) {}
        void run(std::chrono::nanoseconds wait, unsigned long nr);       
};

template <class T, class P>
void PeriodicTimerByTimeCheck<T, P>::run(std::chrono::nanoseconds wait, unsigned long nr) {

    std::cout << "Starting the timer" << std::endl;          
    unsigned long runs=0;        
    bool ok = true;
    TIME nextSendTime = std::chrono::system_clock::now();
    TIME timenow = std::chrono::system_clock::now();

    P::setNextTriggerTime(timenow, nextSendTime, wait);

    while (ok && runs < nr) {  
        timenow = std::chrono::system_clock::now();
        if (timenow >= nextSendTime) {
          ok = this->task.run(nextSendTime, timenow);
          P::setNextTriggerTime(timenow, nextSendTime, wait);
          ++runs;
          }
    }
    std::cout << "Timer finished" << std::endl;
}
