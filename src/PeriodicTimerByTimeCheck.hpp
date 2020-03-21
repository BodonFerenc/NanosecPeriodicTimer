#pragma once

#include "constant.hpp"
#include "PeriodicTimer.hpp"


template <class T>
class PeriodicTimerByTimeCheck: public PeriodicTimer<T> {
    private:
        virtual void setNextTriggerTime(const TIME& input, 
            TIME& nextTriggerTime, const std::chrono::nanoseconds& wait);

    public:
        PeriodicTimerByTimeCheck<T>(T& t): PeriodicTimer<T>(t) {}
        void run(std::chrono::nanoseconds wait, unsigned long nr);       
};


template <class T>
class PeriodicTimerByTimeCheckStrict: public PeriodicTimerByTimeCheck<T> {
     private:
        inline void setNextTriggerTime(const TIME& input, 
            TIME& nextTriggerTime, const std::chrono::nanoseconds& wait) {
            nextTriggerTime += wait;
    }
    public:
        PeriodicTimerByTimeCheckStrict<T>(T& t): PeriodicTimerByTimeCheck<T>(t) {
            std::cout << "PeriodicTimerByTimeCheckStrict created" << std::endl; 
        };
};

template <class T>
class PeriodicTimerByTimeCheckJumpForward: public PeriodicTimerByTimeCheck<T> {
     private:
        inline void setNextTriggerTime(const TIME& input, 
            TIME& nextTriggerTime, const std::chrono::nanoseconds& wait) {
            nextTriggerTime = input + wait -
                std::chrono::nanoseconds(DURNANO((input + wait).time_since_epoch()) % wait.count()); 
    }

    public:
        PeriodicTimerByTimeCheckJumpForward<T>(T& t): PeriodicTimerByTimeCheck<T>(t) {
            std::cout << "PeriodicTimerByTimeCheckJumpForward created" << std::endl;
        };
};


template <class T>
void PeriodicTimerByTimeCheck<T>::run(std::chrono::nanoseconds wait, unsigned long nr) {

    std::cout << "Starting the timer" << std::endl;          
    unsigned long runs=0;        
    bool ok = true;
    auto nextSendTime = std::chrono::system_clock::now();
    auto timenow = std::chrono::system_clock::now();

    setNextTriggerTime(timenow, nextSendTime, wait);

    while (ok && runs < nr) {  
        timenow = std::chrono::system_clock::now();
        if (timenow >= nextSendTime) {
          ok = this->task.run(nextSendTime, timenow);
          setNextTriggerTime(timenow, nextSendTime, wait);
          ++runs;
          }
    }
    std::cout << "Timer finished" << std::endl;
}
