#include <iostream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include "constant.hpp"
#include "CSVStatLoggerTask.hpp"

using namespace std;

CSVStatLoggerTask::CSVStatLoggerTask(unsigned long triggerNr, const char* argv[]) : filename(argv[0]) {
    latencies.reserve(triggerNr);
    cout << "Nr of expected ticks\t\t" << triggerNr << endl;
}

bool CSVStatLoggerTask::run(const TIME& expected, const TIME& real) {
    static bool initialized = false;    

    latencies.push_back(real - expected);
    if (not initialized) {
        firstTS = real;
        initialized = true;
    }
    lastTS = real;
    return true;
}

CSVStatLoggerTask::~CSVStatLoggerTask() {
    if (latencies.empty()) return;

    cout << "Writing results to file " << filename << endl;  
    ofstream file;
    file.open(filename);

    auto s = accumulate( latencies.begin(), latencies.end(), decltype(latencies)::value_type(0));
    auto average = s.count() / latencies.size(); 
    sort(latencies.begin(), latencies.end()); 
    auto median = latencies[latencies.size() / 2];


    file << "realFrequency,maxTimerLatency,avgTimerLatency,medTimerLatency" << endl;
    file << 1000 * 1000 * 1000 * latencies.size() / (lastTS - firstTS).count() << "," 
         << latencies.back().count() << "," 
         << average << ","
         << median.count() << endl;
    file.close();

    cout << "Average wait time latency\t" << average << " nanosec" << endl;

    const float realdur = (float) (lastTS - firstTS).count() / BILLION;    
    cout << "Real duration was\t\t" << realdur << " sec" << endl;
    cout << "Real frequency was\t\t" << (long) (latencies.size() / realdur) << endl;
}