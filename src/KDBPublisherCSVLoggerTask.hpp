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
#include "CSVLoggerTask.hpp"
#include "KDBPublisher.hpp"
#include "KDBSymGenerator.hpp"

template<bool FLUSH>
class KDBPublisherCSVLoggerTask: public CSVLoggerTask, public KDBPublisher {
    protected:  
        KDBSymGenerator symGenerator;
        
        // fields to send. We dont really care about the content.
        char stop = 'i';            /* charactor example */
        int size = 444;             /* long example */
        double price = 12.12;       /* double example */
        char condition = 'c';      

    public: 
        KDBPublisherCSVLoggerTask(unsigned long, const char* argv[]);
        bool run(const TIME&, const TIME&);
};

template<bool FLUSH>
KDBPublisherCSVLoggerTask<FLUSH>::KDBPublisherCSVLoggerTask(unsigned long triggerNr, const char* argv[]) 
    : CSVLoggerTask(triggerNr, argv), KDBPublisher(triggerNr, argv+1), symGenerator{triggerNr} {  
    tableName = (ks((S) "trade"));    
}

template<bool FLUSH>
bool inline KDBPublisherCSVLoggerTask<FLUSH>::run(const TIME& expected, const TIME& real) {
    unsigned long sq = expectedTriggerTimes.size();

    K row = knk(7, ks(*symGenerator.sym_it), kj(sq), kc(stop), ki(size), kf(price), kc(condition), 
        ktj(-KP, DURNANO((std::chrono::system_clock::now() - kdb_start).time_since_epoch())));

    bool res = sendUpdate<FLUSH>(row);
    ++symGenerator.sym_it;

    CSVLoggerTask::run(expected, real);
    return res;
}

