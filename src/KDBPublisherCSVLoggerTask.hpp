/**
 * \class KDBPublisherCSVLoggerTask
 *
 *
 * \brief Task that sends single row updates to Kdb+.
 *
 * This class memorizes all expected and real trigger times
 * and writes to a CSV file upon destruction.
 *
 *
 * \author Ferenc Bodon
 *
 */

#pragma once

#include <chrono>
#include <vector>
#include <array>
#include "CSVStatLoggerTask.hpp"
#include "KDBPublisher.hpp"
#include "KDBSymGenerator.hpp"


// The order of parent classes matters to make sure 
// that kdb+ connection is closed first during destruction.
// Destructors are called in reverse order of declaration

template<bool FLUSH>
class KDBPublisherCSVLoggerTask: public CSVStatLoggerTask, public KDBPublisher {
    protected:  
        KDBSymGenerator symGenerator;
        
        // fields to send. We dont really care about the content.
        char stop = 'i';            /* charactor example */
        int size = 444;             /* long example */
        double price = 12.12;       /* double example */

    public: 
        KDBPublisherCSVLoggerTask(unsigned long, const char* argv[]);
        bool run(const TIME&, const TIME&);
};

template<bool FLUSH>
KDBPublisherCSVLoggerTask<FLUSH>::KDBPublisherCSVLoggerTask(unsigned long triggerNr, const char* argv[]) 
    : CSVStatLoggerTask(triggerNr, argv), KDBPublisher(triggerNr, argv+1), symGenerator{triggerNr} {  
    tableName = (ks((S) "trade"));    
}

template<bool FLUSH>
bool inline KDBPublisherCSVLoggerTask<FLUSH>::run(const TIME& expected, const TIME& real) {
    unsigned long sq = latencies.size();

    K row = knk(7, ks(*symGenerator.sym_it), kj(sq), kc(stop), ki(size), kf(price), kj(sq), 
        ktj(-KP, DURNANO((std::chrono::system_clock::now() - kdb_start).time_since_epoch())));

    bool res = sendUpdate<FLUSH>(row);
    ++symGenerator.sym_it;

    CSVStatLoggerTask::run(expected, real);
    return res;
}

