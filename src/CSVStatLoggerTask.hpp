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

#pragma once

#include <string>
#include <vector>
#include "constant.hpp"


class CSVStatLoggerTask {
    private:  
        std::string filename;
    protected:
        std::vector<std::chrono::nanoseconds> latencies;
        TIME firstTS;
        TIME lastTS;

    public: 
        CSVStatLoggerTask(unsigned long, const char* argv[]);
        auto getSize() {return latencies.size();}
        bool run(const TIME&, const TIME&);
        ~CSVStatLoggerTask();
};

