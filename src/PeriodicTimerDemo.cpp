#include <string>
#include <iostream>
#include <stdlib.h> 
#include <chrono>
#include <functional>
#include "constant.hpp"
#include "CSVLoggerTask.hpp"
#include "Timers.hpp"



using namespace std;

int main(int argc, const char* argv[])
{
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " type freq dur outputfile" << endl;
        return 1;
    }

    const auto freq = atol(argv[2]);          // frequency
    chrono::nanoseconds wait(BILLION / freq);          // wait time between ticks
    cout << "Wait time is set to\t\t" << wait.count() << " nanosec" << endl;
    const auto dur = atoi(argv[3]);            // duration is second

    const auto maxrun = dur * freq;
    cout << "Nr of expected ticks\t\t" << maxrun << endl;

    CSVLoggerTask task(maxrun, argv[4]);
    string timertype(argv[1]);


    std::function<void(CSVLoggerTask&, std::chrono::nanoseconds, unsigned long)> timer;
    if(0 == timertype.compare("bysleep"))
        timer = bysleep<CSVLoggerTask>;
    else if(0 == timertype.compare("bytimerstrict"))
        timer = std::bind(bytimecheck<CSVLoggerTask>, strict, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    else if(0 == timertype.compare("bytimerjumpforward"))
        timer = std::bind(bytimecheck<CSVLoggerTask>, jumpforward, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    else {
        cerr << "Invalid timer type " << timertype << 
            ".\nAvailable options: bysleep, bytimerstrict, bytimerjumpforward" << endl;
        return 1;
    }     

    timer(task, wait, maxrun);

    return 0;
}