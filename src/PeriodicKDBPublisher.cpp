#include <string>
#include <iostream>
#include <stdlib.h> 
#include <chrono>
#include <functional>

#include "constant.hpp"
#include "KDBTradePublisher.hpp"
#include "Timers.hpp"

using namespace std;

int main(int argc, const char* argv[])
{
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << "freq dur host port" << endl;
        return 1;
    }

    const unsigned long freq = atol(argv[1]);          // frequency
    chrono::nanoseconds wait(BILLION / freq);          // wait time between ticks
    cout << "Wait time is set to\t\t" << wait.count() << " nanosec" << endl;
    const unsigned int dur = atoi(argv[2]);            // duration is second

    const unsigned long maxrun = dur * freq;
    cout << "Nr of expected ticks\t\t" << maxrun << endl;

    KDBTradePublisher task(maxrun, argv[3], atoi(argv[4]));

    auto timer = std::bind(bytimecheck<KDBTradePublisher>, strict, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    timer(task, wait, maxrun);    

    return 0;
}