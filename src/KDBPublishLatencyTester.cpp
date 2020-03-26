#include <string>
#include <iostream>
#include <stdlib.h> 
#include <chrono>
#include <sstream>

#include "constant.hpp"
#include "KDBPublisherCSVLoggerTask.hpp"
#include "PeriodicTimerByTimeCheck.hpp"
#include "KDBBatchPublisherCSVLoggerTask.hpp"

using namespace std;

int main(int argc, const char* argv[])
{
    if (argc < 7) {
        cerr << "Usage: " << argv[0] << "[freq] [dur] [outputfile] [host] [port] [flush] [batchsize]" << endl;
        return 1;
    }

    const unsigned long FREQ = atol(argv[1]);          // frequency
    chrono::nanoseconds WAIT(BILLION / FREQ);          // wait time between ticks
    cout << "Wait time is set to\t\t" << WAIT.count() << " nanosec" << endl;
    const unsigned int DUR = atoi(argv[2]);            // duration is second

    const unsigned long MAXRUN = DUR * FREQ;
    unsigned int flush = atoi(argv[6]);
    unsigned int batchsize = atoi(argv[7]);

    if (batchsize == 0) {
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
        cout << "Batch size\t\t\t"<< batchsize << endl;
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
    return 0;
}