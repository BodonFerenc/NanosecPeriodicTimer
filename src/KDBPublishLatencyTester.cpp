#include <string>
#include <iostream>
#include <unistd.h> // for getopt
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


template <typename TTRUE, typename TFALSE>
void startBatchTimer(bool flush, chrono::nanoseconds wait, unsigned long maxrun, string filename, string host, int port, unsigned int batchsize) {
    if (flush) {
            cout << "flushing is enabled" << endl;
            TTRUE task(maxrun, filename, host, port, batchsize);
            auto timer = std::bind(bytimecheck<TTRUE>, strict,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            timer(task, wait, maxrun);
        }
    else {
            TFALSE task(maxrun, filename, host, port, batchsize);
            auto timer = std::bind(bytimecheck<TFALSE>, strict,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            timer(task, wait, maxrun);
    }
}

template<typename P>
int selectTimer(bool flush, chrono::nanoseconds wait, unsigned long maxrun, string filename, string host, int port, string batchtype, unsigned int batchsize) {
    if (batchtype == "nobatch" || batchsize < 2) {
        cout << "No batching" << endl;
        if (flush) {
            cout << "flushing is enabled" << endl;
            KDBPublisherCSVLoggerTask<P, true> task(maxrun, filename, host, port);
            auto timer = std::bind(bytimecheck<KDBPublisherCSVLoggerTask<P, true>>, strict,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            timer(task, wait, maxrun);
        }
        else {
            KDBPublisherCSVLoggerTask<P, false> task(maxrun, filename, host, port);
            auto timer = std::bind(bytimecheck<KDBPublisherCSVLoggerTask<P, false>>, strict,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            timer(task, wait, maxrun);
        }
    } else {
        cout << "Batch size\t\t\t"<< batchsize << endl;
        if (batchtype == "cache") {
            cout << "Cache N' Batch publisher is created" << endl;
            startBatchTimer<KDBCacheAndSendBatchPublisherCSVLoggerTask<P, true>, KDBCacheAndSendBatchPublisherCSVLoggerTask<P, false>>(flush, wait, maxrun, filename, host, port, batchsize);
        }
        else {
            cout << "Batch publisher is created" << endl;
            startBatchTimer<KDBBatchPublisherCSVLoggerTask<P, true>, KDBBatchPublisherCSVLoggerTask<P, false>>(flush, wait, maxrun, filename, host, port, batchsize);
        }
    }
    return 0;
}



int main(int argc, char* argv[])
{
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << "freq dur outputfile host port flush [statonly] [batchsize] [type]" << endl;
        return 1;
    }

    const unsigned long freq = stoul(argv[1]);                         // frequency
    const chrono::nanoseconds wait(BILLION / freq);                    // wait time between ticks
    cout << "Wait time is set to\t\t" << wait.count() << " nanosec" << endl;
    const unsigned int dur = stoi(argv[2]);                            // duration is second
    const unsigned long maxrun = dur * freq;
    cout << "Nr of expected ticks\t\t" << maxrun << endl;
    auto outputfile = argv[3];
    auto host = argv[4];
    auto port = stoi(argv[5]);


    auto flush=false;
    auto statonly=false;
    string batchtype="nobatch";
    unsigned int batchsize = 0;

    opterr=0;
    int c;
    while ((c = getopt (argc-5, argv+5, "fst:b:")) != -1)
    switch (c) {
      case 'f':
        flush = true;
        break;
      case 's':
        statonly = true;
        break;
      case 't':
        batchtype = optarg;
        if (batchtype != "cache" && batchtype != "batch" && batchtype != "nobatch") {
            cerr << "Unknown batch type (should be either 'nobatch, 'cache' or 'batch'): " << batchtype << endl;
            return 1;
        }
        break;
      case 'b':
        batchsize = stoi(optarg);
        break;
      case '?':
        if (optopt == 't')
          cerr << "Option -" << optopt << " requires an argument." << endl;
        else if (isprint (optopt))
          cerr << "Unknown option -" << optopt << endl;
        else
          cerr << "Unknown option character " << optopt << endl;
        return 1;
        default:
            abort ();
      }

    auto res = statonly ? selectTimer<CSVStatLoggerTask>(flush, wait, maxrun, outputfile, host, port, batchtype, batchsize)
        : selectTimer<CSVLoggerTask>(flush, wait, maxrun, outputfile, host, port, batchtype, batchsize);
    cout << "Timer finished" << endl;
    return res;
}