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

template<class P, bool FLUSH>
class KDBCacheAndSendBatchPublisherCSVLoggerTask: public KDBPublisherCSVLoggerTask<P, FLUSH> {
    private:
        unsigned int batchSize;
        K row;


    public:
        KDBCacheAndSendBatchPublisherCSVLoggerTask(unsigned long triggerNr, std::string filename, std::string host, int port, unsigned int batchsize):
            KDBPublisherCSVLoggerTask<P, FLUSH>{triggerNr, filename, host, port}, batchSize{batchsize} {
        row = knk(7, ktn(KS, batchSize), ktn(KJ, batchSize), ktn(KC, batchSize), ktn(KI, batchSize),
                ktn(KF, batchSize), ktn(KJ, batchSize), ktn(KP, batchSize));
        }
        bool run(const TIME&, const TIME&);
};

template<class P, bool FLUSH>
bool inline KDBCacheAndSendBatchPublisherCSVLoggerTask<P, FLUSH>::run(const TIME& expected, const TIME& real) {
    static unsigned long batchnr = 0;
    static unsigned long batchSq = 0;
    const unsigned long sq = KDBPublisherCSVLoggerTask<P, FLUSH>::getSize();

    auto sym = *KDBPublisherCSVLoggerTask<P, FLUSH>::symGenerator.sym_it;

    kS(kK(row)[0])[batchSq]= sym;
    kJ(kK(row)[1])[batchSq]= sq;
    kC(kK(row)[2])[batchSq]= KDBPublisherCSVLoggerTask<P, FLUSH>::stop;
    kI(kK(row)[3])[batchSq]= KDBPublisherCSVLoggerTask<P, FLUSH>::size;
    kF(kK(row)[4])[batchSq]= KDBPublisherCSVLoggerTask<P, FLUSH>::price;
    kJ(kK(row)[5])[batchSq]= batchnr;
    kJ(kK(row)[6])[batchSq]= DURNANO((real - kdb_start).time_since_epoch());

    ++batchSq;

    if (batchSq == batchSize) {
        KDBPublisherCSVLoggerTask<P, FLUSH>::kdbpublisher.sendUpdate(r1(row));
        ++KDBPublisherCSVLoggerTask<P, FLUSH>::symGenerator.sym_it;
        ++batchnr;
        batchSq=0;
    }

    return P::run(expected, real);
}
