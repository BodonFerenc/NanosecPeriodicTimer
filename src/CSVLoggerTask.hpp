/**
 * \class CSVLoggerTask
 *
 *
 * \brief Task that stores trigger times and writes them to a CSV file
 *
 * This class memorizes all expected and real trigger times
 * and writes to a CSV file upon destruction.
 *
 *
 * \author Ferenc Bodon
 *
 */

#ifndef CSVLOGGERTASK_H
#define CSVLOGGERTASK_H

#include <string>
#include <vector>
#include "Task.hpp"

using namespace std;

class CSVLoggerTask: public Task {
    private:  
        vector<struct timespec> expectedTriggerTimes;
        vector<struct timespec> realTriggerTimes;
        string filename;

    public: 
        CSVLoggerTask(unsigned long, string);
        bool run(const struct timespec&, const struct timespec&);
        ~CSVLoggerTask();
};

#endif
