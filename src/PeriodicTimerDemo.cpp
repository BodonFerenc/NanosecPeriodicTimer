#include <string>
#include <iostream>
#include <stdlib.h> 
#include <chrono>

#include "constant.hpp"
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


    if(0 == timertype.compare("bysleep")) {
        PeriodicTimerBySleep<CSVLoggerTask> timer(task);
        timer.run(WAIT, MAXRUN);
    } else if(0 == timertype.compare("bytimerstrict")) {
        PeriodicTimerByTimeCheckStrict<CSVLoggerTask> timer(task);
        timer.run(WAIT, MAXRUN);
    } else if(0 == timertype.compare("bytimerjumpforward")) {
        PeriodicTimerByTimeCheckJumpForward<CSVLoggerTask> timer(task); 
        timer.run(WAIT, MAXRUN); 
    } else {
        cerr << "Invalid timer type " << timertype << 
            ".\nAvailable options: bysleep, bytimerstrict, bytimerjumpforward" << endl;
        return 1;
    }     

    return 0;
}