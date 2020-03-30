/**
 * \class KDBCacheAndSendBatchPublisherCSVLoggerTask
 *
 *
 * \brief Task stores updates in cache and if cache size reaches a limit it sends a batch updat to Kdb+.
 *
 * This class memorizes all expected and real trigger times
 * and writes to a CSV file upon destruction.
 *
 *
 * \author Ferenc Bodon
 *
 */

#pragma once

#include "KDBPublisherCSVLoggerTask.hpp"

template<bool FLUSH>
class KDBCacheAndSendBatchPublisherCSVLoggerTask: public KDBPublisherCSVLoggerTask<FLUSH> {
    private:  
        unsigned int batchSize; 
        K row;
        void initRow();     

    public: 
        KDBCacheAndSendBatchPublisherCSVLoggerTask(unsigned long triggerNr, const char* argv[]): 
            KDBPublisherCSVLoggerTask<FLUSH>(triggerNr, argv), batchSize(atoi(argv[4])) {
            initRow();
        }
        bool run(const TIME&, const TIME&);
};

template<bool FLUSH>
void inline KDBCacheAndSendBatchPublisherCSVLoggerTask<FLUSH>::initRow() {
    row = knk(7, ktn(KS, batchSize), ktn(KJ, batchSize), ktn(KC, batchSize), ktn(KI, batchSize), 
        ktn(KF, batchSize), ktn(KC, batchSize), ktn(KP, batchSize)); 
}

template<bool FLUSH>
bool inline KDBCacheAndSendBatchPublisherCSVLoggerTask<FLUSH>::run(const TIME& expected, const TIME& real) {
    static unsigned int batchCounter = 0;
    static unsigned int sq = 0;

    auto sym = *KDBPublisherCSVLoggerTask<FLUSH>::symGenerator.sym_it;

    kS(kK(row)[0])[batchCounter]= sym;        
    kJ(kK(row)[1])[batchCounter]= sq;
    kC(kK(row)[2])[batchCounter]= KDBPublisherCSVLoggerTask<FLUSH>::stop;
    kI(kK(row)[3])[batchCounter]= KDBPublisherCSVLoggerTask<FLUSH>::size;
    kF(kK(row)[4])[batchCounter]= KDBPublisherCSVLoggerTask<FLUSH>::price;
    kC(kK(row)[5])[batchCounter]= KDBPublisherCSVLoggerTask<FLUSH>::condition;
    kJ(kK(row)[6])[batchCounter]= DURNANO((real - kdb_start).time_since_epoch());

    ++batchCounter;

    if (batchCounter == batchSize) {
        KDBPublisher::sendUpdate<FLUSH>(row);
        
        initRow();
        batchCounter = 0;
        ++KDBPublisherCSVLoggerTask<FLUSH>::symGenerator.sym_it; 
        ++sq;
    }

    return CSVLoggerTask::run(expected, real);
}
