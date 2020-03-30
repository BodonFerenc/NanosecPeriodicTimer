#include <string>
#include <iostream>
#include <stdlib.h> 
#include <chrono>
#include <sstream>
#include <functional>

#include "constant.hpp"
#include "KDBPublisherCSVLoggerTask.hpp"
#include "Timers.hpp"
#include "KDBBatchPublisherCSVLoggerTask.hpp"
#include "KDBCacheAndSendBatchPublisherCSVLoggerTask.hpp"

using namespace std;

int main(int argc, const char* argv[])
{
    if (argc < 6) {
        cerr << "Usage: " << argv[0] << "[freq] [dur] [outputfile] [host] [port] [flush] [batchsize] [type]" << endl;
        return 1;
    }

    const auto FREQ = atol(argv[1]);          // frequency
    const chrono::nanoseconds WAIT(BILLION / FREQ);          // wait time between ticks
    cout << "Wait time is set to\t\t" << WAIT.count() << " nanosec" << endl;
    const auto DUR = atoi(argv[2]);            // duration is second

    const auto MAXRUN = DUR * FREQ;
    const auto flush = atoi(argv[6]);

    if (argc < 6 || atoi(argv[7]) < 2) {
        cout << "No batching" << endl;        
        if (flush) {
            cout << "flushing is enabled" << endl;
            KDBPublisherCSVLoggerTask<true> task(MAXRUN, argv + 3);
            auto timer = std::bind(bytimecheck<KDBPublisherCSVLoggerTask<true>>, strict, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            timer(task, WAIT, MAXRUN);    
        }
        else {
            KDBPublisherCSVLoggerTask<false> task(MAXRUN, argv + 3);
            auto timer = std::bind(bytimecheck<KDBPublisherCSVLoggerTask<false>>, strict, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            timer(task, WAIT, MAXRUN);    
        }
    } else {
        cout << "Batch size\t\t\t"<< atoi(argv[7]) << endl;
        if (argc < 7 || 0 == string(argv[8]).compare("cache")) {
            cout << "Cache N' Batch publisher is created" << endl;            
            if (flush) {
                cout << "flushing is enabled" << endl;
                KDBCacheAndSendBatchPublisherCSVLoggerTask<true> task(MAXRUN, argv + 3);
                auto timer = std::bind(bytimecheck<KDBCacheAndSendBatchPublisherCSVLoggerTask<true>>, strict, 
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                timer(task, WAIT, MAXRUN);    
            }
            else {
                KDBCacheAndSendBatchPublisherCSVLoggerTask<false> task(MAXRUN, argv + 3);
                auto timer = std::bind(bytimecheck<KDBCacheAndSendBatchPublisherCSVLoggerTask<false>>, strict, 
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                timer(task, WAIT, MAXRUN);    
            }            
        } else if (0 == string(argv[8]).compare("batch")) {
            cout << "Batch publisher is created" << endl;
            if (flush) {
                cout << "flushing is enabled" << endl;
                KDBBatchPublisherCSVLoggerTask<true> task(MAXRUN, argv + 3);
                auto timer = std::bind(bytimecheck<KDBBatchPublisherCSVLoggerTask<true>>, strict, 
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                timer(task, WAIT, MAXRUN);    
            }
            else {
                KDBBatchPublisherCSVLoggerTask<false> task(MAXRUN, argv + 3);
                auto timer = std::bind(bytimecheck<KDBBatchPublisherCSVLoggerTask<false>>, strict, 
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                timer(task, WAIT, MAXRUN);    
            }
        }
        else {
            cerr << "Unknown batch type (should be 'cache' or 'batch'): " << argv[8] << endl;
            return 1;
        }
    }
    return 0;
}