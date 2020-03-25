#include <string>
#include <iostream>
#include <stdlib.h> 
#include <chrono>

#include "constant.hpp"
#include "PeriodicTimer.hpp"
#include "CSVLoggerTask.hpp"
#include "PeriodicTimerBySleep.hpp"
#include "PeriodicTimerByTimeCheck.hpp"



using namespace std;

int main(int argc, const char* argv[])
{
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " [type] [freq] [dur] [outputfile]" << endl;
        return 1;
    }

    const unsigned long FREQ = atol(argv[2]);          // frequency
    chrono::nanoseconds WAIT(BILLION / FREQ);          // wait time between ticks
    cout << "Wait time is set to\t\t" << WAIT.count() << " nanosec" << endl;
    const unsigned int DUR = atoi(argv[3]);            // duration is second

    const unsigned long MAXRUN = DUR * FREQ;

    CSVLoggerTask task(MAXRUN, argv + 4);
    string timertype(argv[1]);


    PeriodicTimer<CSVLoggerTask> *timer;
    if(0 == timertype.compare("bysleep"))
        timer = new PeriodicTimerBySleep<CSVLoggerTask>(task);
    else if(0 == timertype.compare("bytimerstrict"))
        timer = new PeriodicTimerByTimeCheck<CSVLoggerTask, Strict>(task);
    else if(0 == timertype.compare("bytimerjumpforward"))
        timer = new PeriodicTimerByTimeCheck<CSVLoggerTask, JumpForward>(task); 
    else {
        cerr << "Invalid timer type " << timertype << 
            ".\nAvailable options: bysleep, bytimerstrict, bytimerjumpforward" << endl;
        return 1;
    }     

    timer->run(WAIT, MAXRUN);

    return 0;
}