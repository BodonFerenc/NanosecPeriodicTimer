#include <iostream>
#include <fstream>
#include "constant.hpp"
#include "CSVLoggerTask.hpp"

using namespace std;

CSVLoggerTask::CSVLoggerTask(unsigned long triggerNr, string fname) : filename(fname) {
    expectedTriggerTimes.reserve(triggerNr);
    realTriggerTimes.reserve(triggerNr);
    cout << "Nr of expected ticks\t\t" << triggerNr << endl;
}

bool CSVLoggerTask::run(const struct timespec& expected, const struct timespec& real) {
    expectedTriggerTimes.push_back(expected);
    realTriggerTimes.push_back(real);
    return true;
}

CSVLoggerTask::~CSVLoggerTask() {
    cout << "Writing results to file " << filename << endl;  
    ofstream file;
    file.open(filename);

    file << "planned,real,latency" << endl;
    struct timespec sumwait = {0, 0};
    for (unsigned long i=0; i < expectedTriggerTimes.size(); ++i) {
      file << timespecToNano(expectedTriggerTimes[i]) << "," << timespecToNano(realTriggerTimes[i]) 
        << "," << BILLION * (realTriggerTimes[i].tv_sec - expectedTriggerTimes[i].tv_sec) + realTriggerTimes[i].tv_nsec - expectedTriggerTimes[i].tv_nsec << "\n";
      sumwait.tv_sec += realTriggerTimes[i].tv_sec - expectedTriggerTimes[i].tv_sec;
      sumwait.tv_nsec += realTriggerTimes[i].tv_nsec - expectedTriggerTimes[i].tv_nsec;
    }
    file.close();

    sumwait.tv_sec += sumwait.tv_sec / BILLION;
    sumwait.tv_nsec = sumwait.tv_nsec % BILLION;

    cout << "Average wait time latency\t" << timespecToNano(sumwait) / expectedTriggerTimes.size() << " nanosec" << endl;

    const float realdur = realTriggerTimes.back().tv_sec - expectedTriggerTimes.front().tv_sec + 
        (float) (realTriggerTimes.back().tv_nsec - expectedTriggerTimes.front().tv_nsec) / BILLION;    
    cout << "Real duration was\t\t" << realdur << " sec" << endl;
    cout << "Real frequency was\t\t" << (long) (expectedTriggerTimes.size() / realdur) << endl;

}
