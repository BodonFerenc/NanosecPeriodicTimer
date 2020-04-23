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
#include "KDBPublisher.hpp"
#include "KDBSymGenerator.hpp"


template<class P, bool FLUSH>
class KDBPublisherCSVLoggerTask: public P {
    protected:  
        KDBSymGenerator symGenerator;
        
        // fields to send. We dont really care about the content.
        char stop = 'i';            /* charactor example */
        int size = 444;             /* long example */
        double price = 12.12;       /* double example */

// The order of data members matters!
// The kdb+ connection needs to be closed as a first task during destruction.
// This allows getting an accurate upper bound for the kdb+ insert time.
// Hence KDBPublisher must be the last data member.

        KDBPublisher kdbpublisher;

    public: 
        KDBPublisherCSVLoggerTask(unsigned long, const char* argv[]);
        bool run(const TIME&, const TIME&);
};

template<class P, bool FLUSH>
KDBPublisherCSVLoggerTask<P, FLUSH>::KDBPublisherCSVLoggerTask(unsigned long triggerNr, const char* argv[]) 
    :P{triggerNr, argv}, symGenerator{triggerNr}, kdbpublisher(triggerNr, argv+1) {  
    kdbpublisher.tableName = (ks((S) "trade"));    
}

template<class P, bool FLUSH>
bool inline KDBPublisherCSVLoggerTask<P, FLUSH>::run(const TIME& expected, const TIME& real) {
    unsigned long sq = P::size();

    K row = knk(7, ks(*symGenerator.sym_it), kj(sq), kc(stop), ki(size), kf(price), kj(sq), 
        ktj(-KP, DURNANO((std::chrono::system_clock::now() - kdb_start).time_since_epoch())));

    bool res = kdbpublisher.sendUpdate<FLUSH>(row);
    ++symGenerator.sym_it;

    P::run(expected, real);
    return res;
}

