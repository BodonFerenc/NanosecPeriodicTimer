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

template<class P, bool FLUSH>
class KDBBatchPublisherCSVLoggerTask: public KDBPublisherCSVLoggerTask<P, FLUSH> {
    private:  
        unsigned int batchSize;      

    public: 
        KDBBatchPublisherCSVLoggerTask(unsigned long triggerNr, const char* argv[]) : 
            KDBPublisherCSVLoggerTask<P, FLUSH>(triggerNr, argv), batchSize(atoi(argv[5])) {}
        bool run(const TIME&, const TIME&);
};

template<class P, bool FLUSH>
bool inline KDBBatchPublisherCSVLoggerTask<P, FLUSH>::run(const TIME& expected, const TIME& real) {
    auto sym = *KDBPublisherCSVLoggerTask<P, FLUSH>::symGenerator.sym_it;
    static unsigned long sq = 0;
    const unsigned long batchnr = KDBPublisherCSVLoggerTask<P, FLUSH>::getSize();

    const unsigned long realTriggerTime = DURNANO((real - kdb_start).time_since_epoch());

    K row = knk(7, ktn(KS, batchSize), ktn(KJ, batchSize), ktn(KC, batchSize), ktn(KI, batchSize), 
        ktn(KF, batchSize), ktn(KJ, batchSize), ktn(KP, batchSize));

    ++KDBPublisherCSVLoggerTask<P, FLUSH>::symGenerator.sym_it; 

    --batchSize;        // not an elegant solution!
    for(decltype(batchSize) batchSq = 0; batchSq < batchSize; ++batchSq) {
        kS(kK(row)[0])[batchSq]= sym;        
        kJ(kK(row)[1])[batchSq]= sq++;
        kC(kK(row)[2])[batchSq]= KDBPublisherCSVLoggerTask<P, FLUSH>::stop;
        kI(kK(row)[3])[batchSq]= KDBPublisherCSVLoggerTask<P, FLUSH>::getSize;
        kF(kK(row)[4])[batchSq]= KDBPublisherCSVLoggerTask<P, FLUSH>::price;
        kJ(kK(row)[5])[batchSq]= batchnr;
        kJ(kK(row)[6])[batchSq]= realTriggerTime;
    }

    kS(kK(row)[0])[batchSize]= sym;        
    kJ(kK(row)[1])[batchSize]= sq++;
    kC(kK(row)[2])[batchSize]= KDBPublisherCSVLoggerTask<P, FLUSH>::stop;
    kI(kK(row)[3])[batchSize]= KDBPublisherCSVLoggerTask<P, FLUSH>::getSize;
    kF(kK(row)[4])[batchSize]= KDBPublisherCSVLoggerTask<P, FLUSH>::price;
    kJ(kK(row)[5])[batchSize]= batchnr;
    kJ(kK(row)[6])[batchSize]= DURNANO((std::chrono::system_clock::now() - kdb_start).time_since_epoch());

    ++batchSize;

    KDBPublisherCSVLoggerTask<P, FLUSH>::kdbpublisher.sendUpdate(row);

    return P::run(expected, real);
}
