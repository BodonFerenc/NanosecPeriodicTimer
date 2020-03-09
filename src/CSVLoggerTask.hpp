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


class CSVLoggerTask: public Task {
    private:  
        vector<TIME> expectedTriggerTimes;
        vector<TIME> realTriggerTimes;
        string filename;

    public: 
        CSVLoggerTask(unsigned long, const char* argv[]);
        bool run(const TIME&, const TIME&);
        ~CSVLoggerTask();
};

#endif
