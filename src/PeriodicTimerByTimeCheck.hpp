#ifndef PERIODICTIMERBYTIMECHECK_H
#define PERIODICTIMERBYTIMECHECK_H

#include <time.h>
#include "constant.hpp"
#include "PeriodicTimer.hpp"


template <class T>
class PeriodicTimerByTimeCheck: public PeriodicTimer<T> {
    private:
        virtual void setNextTriggerTime(const struct timespec& input, struct timespec& nextTriggerTime, long nanosec) = 0;

    public:
        PeriodicTimerByTimeCheck<T>(T& t): PeriodicTimer<T>(t) {}
        bool isafter(const struct timespec& t1, const struct timespec& t2);
        void run(unsigned long wait, unsigned long nr);       
};


template <class T>
class PeriodicTimerByTimeCheckStrict: public PeriodicTimerByTimeCheck<T> {
     private:
        void setNextTriggerTime(const struct timespec& input, struct timespec& nextTriggerTime, long nanosec) {
            // we ignore current time
            nextTriggerTime.tv_nsec += nanosec; 
            if (nextTriggerTime.tv_nsec >= BILLION) {
                ++(nextTriggerTime.tv_sec);
                nextTriggerTime.tv_nsec -= BILLION;
            } 
    }
    public:
        PeriodicTimerByTimeCheckStrict<T>(T& t): PeriodicTimerByTimeCheck<T>(t) {
            cout << "PeriodicTimerByTimeCheckStrict created" << endl; 
        };
};

template <class T>
class PeriodicTimerByTimeCheckJumpForward: public PeriodicTimerByTimeCheck<T> {
     private:
        void setNextTriggerTime(const struct timespec& input, struct timespec& nextTriggerTime, long nanosec) {
            nextTriggerTime.tv_nsec = input.tv_nsec + nanosec - input.tv_nsec % nanosec;

            if (nextTriggerTime.tv_nsec >= BILLION) {
                nextTriggerTime.tv_sec = input.tv_sec + 1;
                nextTriggerTime.tv_nsec -= BILLION;        
            } else {
                nextTriggerTime.tv_sec = input.tv_sec;
            }
    }

    public:
        PeriodicTimerByTimeCheckJumpForward<T>(T& t): PeriodicTimerByTimeCheck<T>(t) {
            cout << "PeriodicTimerByTimeCheckJumpForward created" << endl;
        };
};

/** returns true if the first time is at or after the second time
*/
template <class T>
bool PeriodicTimerByTimeCheck<T>::isafter(const struct timespec& t1, const struct timespec& t2) {
    return t1.tv_sec > t2.tv_sec || 
        (t1.tv_sec == t2.tv_sec && t1.tv_nsec >= t2.tv_nsec);
}

template <class T>
void PeriodicTimerByTimeCheck<T>::run(unsigned long wait, unsigned long nr) {

    cout << "Starting the timer" << endl;          
    unsigned long runs=0;        
    bool ok = true;
    struct timespec nextSendTime;
    clock_gettime(CLOCK_MONOTONIC,&nextSendTime);

    struct timespec timenow;    
    clock_gettime(CLOCK_MONOTONIC,&timenow);

    setNextTriggerTime(timenow, nextSendTime, wait);

    while (ok && runs < nr) {  
        clock_gettime(CLOCK_MONOTONIC, &timenow);
        if (isafter(timenow, nextSendTime)) {
          ok = this->task.run(nextSendTime, timenow);
          setNextTriggerTime(timenow, nextSendTime, wait);
          ++runs;
          }
    }
    cout << "Timer finished" << endl;
}

#endif