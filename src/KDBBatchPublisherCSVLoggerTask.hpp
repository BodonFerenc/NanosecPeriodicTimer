/**
 * \class KDBBatchPublisherCSVLoggerTask
 *
 *
 * \brief Task sends batch update to Kdb+.
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
class KDBBatchPublisherCSVLoggerTask: public KDBPublisherCSVLoggerTask<FLUSH> {
    private:  
        unsigned int batchSize;      

    public: 
        KDBBatchPublisherCSVLoggerTask(unsigned long triggerNr, const char* argv[]) : 
            KDBPublisherCSVLoggerTask<FLUSH>(triggerNr, argv), batchSize(atoi(argv[4])) {}
        bool run(const TIME&, const TIME&);
};

template<bool FLUSH>
bool inline KDBBatchPublisherCSVLoggerTask<FLUSH>::run(const TIME& expected, const TIME& real) {
    auto sym = *KDBPublisherCSVLoggerTask<FLUSH>::symGenerator.sym_it;
    unsigned long sq = CSVLoggerTask::expectedTriggerTimes.size();

    unsigned long realTriggerTime = DURNANO((real - kdb_start).time_since_epoch());

    K row = knk(7, ktn(KS, batchSize), ktn(KJ, batchSize), ktn(KC, batchSize), ktn(KI, batchSize), 
        ktn(KF, batchSize), ktn(KC, batchSize), ktn(KP, batchSize));

    ++KDBPublisherCSVLoggerTask<FLUSH>::symGenerator.sym_it; 

    --batchSize;

    for(unsigned int i = 0; i < batchSize; ++i) {
        kS(kK(row)[0])[i]= sym;        
        kJ(kK(row)[1])[i]= sq;
        kC(kK(row)[2])[i]= KDBPublisherCSVLoggerTask<FLUSH>::stop;
        kI(kK(row)[3])[i]= KDBPublisherCSVLoggerTask<FLUSH>::size;
        kF(kK(row)[4])[i]= KDBPublisherCSVLoggerTask<FLUSH>::price;
        kC(kK(row)[5])[i]= KDBPublisherCSVLoggerTask<FLUSH>::condition;
        kJ(kK(row)[6])[i]= realTriggerTime;
    }

    kS(kK(row)[0])[batchSize]= sym;        
    kJ(kK(row)[1])[batchSize]= sq;
    kC(kK(row)[2])[batchSize]= KDBPublisherCSVLoggerTask<FLUSH>::stop;
    kI(kK(row)[3])[batchSize]= KDBPublisherCSVLoggerTask<FLUSH>::size;
    kF(kK(row)[4])[batchSize]= KDBPublisherCSVLoggerTask<FLUSH>::price;
    kC(kK(row)[5])[batchSize]= KDBPublisherCSVLoggerTask<FLUSH>::condition;
    kJ(kK(row)[6])[batchSize]= DURNANO((std::chrono::system_clock::now() - kdb_start).time_since_epoch());

    ++batchSize;

    KDBPublisher::sendUpdate<FLUSH>(row);

    return CSVLoggerTask::run(expected, real);
}
