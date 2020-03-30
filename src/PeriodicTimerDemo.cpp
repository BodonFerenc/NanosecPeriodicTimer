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
        cerr << "Usage: " << argv[0] << " [type] [freq] [dur] [outputfile]" << endl;
        return 1;
    }

    const auto FREQ = atol(argv[2]);          // frequency
    chrono::nanoseconds WAIT(BILLION / FREQ);          // wait time between ticks
    cout << "Wait time is set to\t\t" << WAIT.count() << " nanosec" << endl;
    const auto DUR = atoi(argv[3]);            // duration is second

    const auto MAXRUN = DUR * FREQ;

    CSVLoggerTask task(MAXRUN, argv + 4);
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

    timer(task, WAIT, MAXRUN);

    return 0;
}