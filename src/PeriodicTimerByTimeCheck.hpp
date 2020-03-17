#pragma once

#include "constant.hpp"
#include "PeriodicTimer.hpp"


template <class T>
class PeriodicTimerByTimeCheck: public PeriodicTimer<T> {
    private:
        virtual void setNextTriggerTime(const TIME& input, 
            TIME& nextTriggerTime, const chrono::nanoseconds& wait);

    public:
        PeriodicTimerByTimeCheck<T>(T& t): PeriodicTimer<T>(t) {}
        void run(chrono::nanoseconds wait, unsigned long nr);       
};


template <class T>
class PeriodicTimerByTimeCheckStrict: public PeriodicTimerByTimeCheck<T> {
     private:
        inline void setNextTriggerTime(const TIME& input, 
            TIME& nextTriggerTime, const chrono::nanoseconds& wait) {
            nextTriggerTime += wait;
    }
    public:
        PeriodicTimerByTimeCheckStrict<T>(T& t): PeriodicTimerByTimeCheck<T>(t) {
            cout << "PeriodicTimerByTimeCheckStrict created" << endl; 
        };
};

template <class T>
class PeriodicTimerByTimeCheckJumpForward: public PeriodicTimerByTimeCheck<T> {
     private:
        inline void setNextTriggerTime(const TIME& input, 
            TIME& nextTriggerTime, const chrono::nanoseconds& wait) {
            nextTriggerTime = input + wait -
                chrono::nanoseconds(DURNANO((input + wait).time_since_epoch()) % wait.count()); 
    }

    public:
        PeriodicTimerByTimeCheckJumpForward<T>(T& t): PeriodicTimerByTimeCheck<T>(t) {
            cout << "PeriodicTimerByTimeCheckJumpForward created" << endl;
        };
};


template <class T>
void PeriodicTimerByTimeCheck<T>::run(chrono::nanoseconds wait, unsigned long nr) {

    cout << "Starting the timer" << endl;          
    unsigned long runs=0;        
    bool ok = true;
    auto nextSendTime = chrono::system_clock::now();
    auto timenow = chrono::system_clock::now();

    setNextTriggerTime(timenow, nextSendTime, wait);

    while (ok && runs < nr) {  
        timenow = chrono::system_clock::now();
        if (timenow >= nextSendTime) {
          ok = this->task.run(nextSendTime, timenow);
          setNextTriggerTime(timenow, nextSendTime, wait);
          ++runs;
          }
    }
    cout << "Timer finished" << endl;
}
