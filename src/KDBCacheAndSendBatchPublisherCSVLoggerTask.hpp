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
        ktn(KF, batchSize), ktn(KJ, batchSize), ktn(KP, batchSize)); 
}

template<bool FLUSH>
bool inline KDBCacheAndSendBatchPublisherCSVLoggerTask<FLUSH>::run(const TIME& expected, const TIME& real) {
    static unsigned long batchnr = 0;
    static unsigned long batchSq = 0;
    const unsigned long sq = CSVLoggerTask::expectedTriggerTimes.size();

    auto sym = *KDBPublisherCSVLoggerTask<FLUSH>::symGenerator.sym_it;

    kS(kK(row)[0])[batchSq]= sym;        
    kJ(kK(row)[1])[batchSq]= sq;
    kC(kK(row)[2])[batchSq]= KDBPublisherCSVLoggerTask<FLUSH>::stop;
    kI(kK(row)[3])[batchSq]= KDBPublisherCSVLoggerTask<FLUSH>::size;
    kF(kK(row)[4])[batchSq]= KDBPublisherCSVLoggerTask<FLUSH>::price;
    kJ(kK(row)[5])[batchSq]= batchnr;
    kJ(kK(row)[6])[batchSq]= DURNANO((real - kdb_start).time_since_epoch());

    ++batchSq;

    if (batchSq == batchSize) {
        KDBPublisher::sendUpdate<FLUSH>(row);    
        initRow();
        ++KDBPublisherCSVLoggerTask<FLUSH>::symGenerator.sym_it; 
        ++batchnr;
        batchSq=0;
    }

    return CSVStatLoggerTask::run(expected, real);
}
