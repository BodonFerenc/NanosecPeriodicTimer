#ifndef PERIODICTIMERBYSLEEP_H
#define PERIODICTIMERBYSLEEP_H

#include <time.h>

#include "constant.hpp"
#include "PeriodicTimer.hpp"


template <class T>
class PeriodicTimerBySleep: public PeriodicTimer<T> {
    private:
        void setNextTriggerTime(const struct timespec& input, struct timespec& nextTriggerTime, long nanosec);

    public:
        PeriodicTimerBySleep<T>(T& t): PeriodicTimer<T>(t){ 
            cout << "PeriodicTimerBySleep created" << endl;
        }
        void run(unsigned long wait, unsigned long nr);
};

template <class T>
void PeriodicTimerBySleep<T>::setNextTriggerTime(const struct timespec& input, 
    struct timespec& nextTriggerTime, long nanosec) {
            // we ignore current time
            nextTriggerTime.tv_nsec = input.tv_nsec + nanosec; 
            if (nextTriggerTime.tv_nsec >= BILLION) {
                nextTriggerTime.tv_sec = input.tv_sec + 1;
                nextTriggerTime.tv_nsec -= BILLION;
            } else {
                nextTriggerTime.tv_sec = input.tv_sec;
            }
    } 

template <class T>    
void PeriodicTimerBySleep<T>::run(unsigned long wait, unsigned long nr) {

    cout << "Starting the timer" << endl;   
    unsigned long runs=0;   
    bool ok = true;
    struct timespec timenow;
    struct timespec nextSendTime;
    clock_gettime(CLOCK_MONOTONIC,&timenow);

    while (ok && runs++ < nr) {        
        setNextTriggerTime(timenow, nextSendTime, wait);
        clock_nanosleep(CLOCK_MONOTONIC, 1, &nextSendTime, NULL);
        clock_gettime(CLOCK_MONOTONIC, &timenow);
        ok = this->task.run(nextSendTime, timenow);     
    } 
    cout << "Timer finished" << endl;
}

#endif