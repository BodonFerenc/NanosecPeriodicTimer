#include <chrono>
#include <thread>
#include "constant.hpp"

template <class T>    
void bysleep(T& task, std::chrono::nanoseconds wait, unsigned long nr) {

    std::cout << "Starting sleep-based timer" << std::endl;   
    unsigned long runs=0;   
    bool ok = true;
    auto timenow = std::chrono::system_clock::now();
    TIME nextTriggerTime;

    while (ok && runs++ < nr) {        
        nextTriggerTime = timenow + wait;
        std::this_thread::sleep_until(nextTriggerTime);
        timenow = std::chrono::system_clock::now();
        ok = task.run(nextTriggerTime, timenow);
    } 
    std::cout << "Timer finished" << std::endl;
}

TIME strict(const TIME& input, const TIME& nextTriggerTime, const std::chrono::nanoseconds& wait) {
    return nextTriggerTime + wait;
    }

TIME jumpforward(const TIME& input, const TIME& nextTriggerTime, const std::chrono::nanoseconds& wait) {
    return input + wait -
                std::chrono::nanoseconds(DURNANO((input + wait).time_since_epoch()) % wait.count());
    }



template <class T>
void bytimecheck(std::function<TIME(const TIME&, const TIME&, const std::chrono::nanoseconds&)> nexttimeSetter, 
    T& task, std::chrono::nanoseconds wait, unsigned long nr) {

    std::cout << "Starting timecheck-based timer" << std::endl;          
    unsigned long runs=0;        
    bool ok = true;
    TIME nextTriggerTime = std::chrono::system_clock::now();
    TIME timenow = std::chrono::system_clock::now();

    nextTriggerTime = nexttimeSetter(timenow, nextTriggerTime, wait);

    while (ok && runs < nr) {  
        timenow = std::chrono::system_clock::now();
        if (timenow >= nextTriggerTime) {
          ok = task.run(nextTriggerTime, timenow);
          nextTriggerTime = nexttimeSetter(timenow, nextTriggerTime, wait);
          ++runs;
          }
    }
    std::cout << "Timer finished" << std::endl;
}
