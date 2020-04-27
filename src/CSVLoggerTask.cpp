#include <iostream>
#include <fstream>
#include "constant.hpp"
#include "CSVLoggerTask.hpp"

using namespace std;

CSVLoggerTask::CSVLoggerTask(unsigned long triggerNr, string filename) : filename{filename} {
    expectedTriggerTimes.reserve(triggerNr);
    realTriggerTimes.reserve(triggerNr);
}

bool CSVLoggerTask::run(const TIME& expected, const TIME& real) {
    expectedTriggerTimes.push_back(expected);
    realTriggerTimes.push_back(real);
    return true;
}

CSVLoggerTask::~CSVLoggerTask() {
    if (expectedTriggerTimes.empty()) return;

    cout << "Writing results to file " << filename << endl;  
    ofstream file;
    file.open(filename);

    file << "planned,real,latency" << endl;
    std::chrono::nanoseconds sumwait(0);
    for (unsigned long i=0; i < expectedTriggerTimes.size(); ++i) {
      file << DURNANO(expectedTriggerTimes[i].time_since_epoch()) << "," 
           << DURNANO(realTriggerTimes[i].time_since_epoch()) << "," 
           << DURNANO(realTriggerTimes[i] - expectedTriggerTimes[i]) << "\n";
      sumwait += realTriggerTimes[i] - expectedTriggerTimes[i];
    }
    file.close();


    cout << "Average wait time latency\t" << DURNANO(sumwait) / expectedTriggerTimes.size() << " nanosec" << endl;

    const float realdur = (float) DURNANO(realTriggerTimes.back() - expectedTriggerTimes.front()) / BILLION;    
    cout << "Real duration was\t\t" << realdur << " sec" << endl;
    cout << "Real frequency was\t\t" << (long) (expectedTriggerTimes.size() / realdur) << endl;

}
