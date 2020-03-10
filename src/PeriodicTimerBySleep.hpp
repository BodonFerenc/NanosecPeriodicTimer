#pragma once

#include <thread>

#include "constant.hpp"
#include "PeriodicTimer.hpp"


template <class T>
class PeriodicTimerBySleep: public PeriodicTimer<T> {
    private:
        void setNextTriggerTime(const TIME& input, 
            TIME& nextTriggerTime, const chrono::nanoseconds& wait);

    public:
        PeriodicTimerBySleep<T>(T& t): PeriodicTimer<T>(t){ 
            cout << "PeriodicTimerBySleep created" << endl;
        }
        void run(chrono::nanoseconds wait, unsigned long nr);
};

template <class T>
void PeriodicTimerBySleep<T>::setNextTriggerTime(const TIME& input, 
    TIME& nextTriggerTime, const chrono::nanoseconds& wait) {
            nextTriggerTime = input + wait;
    } 

template <class T>    
void PeriodicTimerBySleep<T>::run(chrono::nanoseconds wait, unsigned long nr) {

    cout << "Starting the timer" << endl;   
    unsigned long runs=0;   
    bool ok = true;
    auto timenow = chrono::system_clock::now();
    TIME nextSendTime;

    while (ok && runs++ < nr) {        
        setNextTriggerTime(timenow, nextSendTime, wait);
        this_thread::sleep_until(nextSendTime);
        timenow = chrono::system_clock::now();
        ok = this->task.run(nextSendTime, timenow);
    } 
    cout << "Timer finished" << endl;
}
