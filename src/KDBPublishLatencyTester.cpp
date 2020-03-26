#include <string>
#include <iostream>
#include <stdlib.h> 
#include <chrono>
#include <sstream>

#include "constant.hpp"
#include "KDBPublisherCSVLoggerTask.hpp"
#include "PeriodicTimerByTimeCheck.hpp"
#include "KDBBatchPublisherCSVLoggerTask.hpp"
#include "KDBCacheAndSendBatchPublisherCSVLoggerTask.hpp"

using namespace std;

int main(int argc, const char* argv[])
{
    if (argc < 6) {
        cerr << "Usage: " << argv[0] << "[freq] [dur] [outputfile] [host] [port] [flush] [batchsize] [type]" << endl;
        return 1;
    }

    const unsigned long FREQ = atol(argv[1]);          // frequency
    chrono::nanoseconds WAIT(BILLION / FREQ);          // wait time between ticks
    cout << "Wait time is set to\t\t" << WAIT.count() << " nanosec" << endl;
    const unsigned int DUR = atoi(argv[2]);            // duration is second

    const unsigned long MAXRUN = DUR * FREQ;
    unsigned int flush = atoi(argv[6]);

    if (argc < 6 || atoi(argv[7]) == 0) {
        cout << "No batching" << endl;        
        if (flush) {
            cout << "flushing is enabled" << endl;
            KDBPublisherCSVLoggerTask<true> task(MAXRUN, argv + 3);
            PeriodicTimerByTimeCheck<KDBPublisherCSVLoggerTask<true>, Strict> timer(task); 
            timer.run(WAIT, MAXRUN);    
        }
        else {
            KDBPublisherCSVLoggerTask<false> task(MAXRUN, argv + 3);
            PeriodicTimerByTimeCheck<KDBPublisherCSVLoggerTask<false>, Strict> timer(task); 
            timer.run(WAIT, MAXRUN);    
        }
    } else {
        cout << "Batch size\t\t\t"<< atoi(argv[7]) << endl;
        if (argc < 7 || 0 == string(argv[8]).compare("cache")) {
            cout << "Cache N' Batch publisher is created" << endl;            
            if (flush) {
                cout << "flushing is enabled" << endl;
                KDBCacheAndSendBatchPublisherCSVLoggerTask<true> task(MAXRUN, argv + 3);
                PeriodicTimerByTimeCheck<KDBCacheAndSendBatchPublisherCSVLoggerTask<true>, Strict> timer(task); 
                timer.run(WAIT, MAXRUN);    
            }
            else {
                KDBCacheAndSendBatchPublisherCSVLoggerTask<false> task(MAXRUN, argv + 3);
                PeriodicTimerByTimeCheck<KDBCacheAndSendBatchPublisherCSVLoggerTask<false>, Strict> timer(task); 
                timer.run(WAIT, MAXRUN);    
            }            
        } else if (0 == string(argv[8]).compare("batch")) {
            cout << "Batch publisher is created" << endl;
            if (flush) {
                cout << "flushing is enabled" << endl;
                KDBBatchPublisherCSVLoggerTask<true> task(MAXRUN, argv + 3);
                PeriodicTimerByTimeCheck<KDBBatchPublisherCSVLoggerTask<true>, Strict> timer(task); 
                timer.run(WAIT, MAXRUN);    
            }
            else {
                KDBBatchPublisherCSVLoggerTask<false> task(MAXRUN, argv + 3);
                PeriodicTimerByTimeCheck<KDBBatchPublisherCSVLoggerTask<false>, Strict> timer(task); 
                timer.run(WAIT, MAXRUN);    
            }
        }
        else cerr << "Unknown batch type (should be 'cache' or 'batch'): " << argv[8] << endl;
        return 1;
    }
    return 0;
}