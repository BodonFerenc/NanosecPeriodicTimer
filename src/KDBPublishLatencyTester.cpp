#include <string>
#include <iostream>
#include <stdlib.h> 
#include <chrono>
#include <sstream>
#include <functional>

#include "constant.hpp"
#include "CSVLoggerTask.hpp"
#include "CSVStatLoggerTask.hpp"
#include "KDBPublisherCSVLoggerTask.hpp"
#include "Timers.hpp"
#include "KDBBatchPublisherCSVLoggerTask.hpp"
#include "KDBCacheAndSendBatchPublisherCSVLoggerTask.hpp"

using namespace std;

template <class TTRUE, class TFALSE>
void startTimer(int argc, const char* argv[]) {
    const auto FREQ = atol(argv[1]);          // frequency
    const chrono::nanoseconds WAIT(BILLION / FREQ);          // wait time between ticks
    cout << "Wait time is set to\t\t" << WAIT.count() << " nanosec" << endl;
    const auto DUR = atoi(argv[2]);            // duration is second

    const auto MAXRUN = DUR * FREQ;
    const auto flush = atoi(argv[6]);

    if (flush) {
            cout << "flushing is enabled" << endl;
            TTRUE task(MAXRUN, argv + 3);
            auto timer = std::bind(bytimecheck<TTRUE>, strict, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            timer(task, WAIT, MAXRUN);    
        }
    else {
            TFALSE task(MAXRUN, argv + 3);
            auto timer = std::bind(bytimecheck<TFALSE>, strict, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            timer(task, WAIT, MAXRUN);    
    }
}

template<typename P>
int selectTimer(int argc, const char* argv[]) {
    if (argc < 9 || atoi(argv[8]) < 2) {
        cout << "No batching" << endl;    
        startTimer<KDBPublisherCSVLoggerTask<P, true>, KDBPublisherCSVLoggerTask<P, false>>(argc, argv);
    } else {
        cout << "Batch size\t\t\t"<< atoi(argv[8]) << endl;
        if (argc < 10 || 0 == string(argv[9]).compare("cache")) {
            cout << "Cache N' Batch publisher is created" << endl;
            startTimer<KDBCacheAndSendBatchPublisherCSVLoggerTask<P, true>, KDBCacheAndSendBatchPublisherCSVLoggerTask<P, false>>(argc, argv);           
        } else if (0 == string(argv[9]).compare("batch")) {
            cout << "Batch publisher is created" << endl;
            startTimer<KDBBatchPublisherCSVLoggerTask<P, true>, KDBBatchPublisherCSVLoggerTask<P, false>>(argc, argv);           
        }
        else {
            cerr << "Unknown batch type (should be 'cache' or 'batch'): " << argv[9] << endl;
            return 1;
        }
    }
    return 0;
}

int main(int argc, const char* argv[])
{
    if (argc < 7) {
        cerr << "Usage: " << argv[0] << "freq dur outputfile host port flush [statonly] [batchsize] [type]" << endl;
        return 1;
    }

    return atoi(argv[7]) ? selectTimer<CSVStatLoggerTask>(argc, argv) 
        : selectTimer<CSVLoggerTask>(argc, argv);
    
}