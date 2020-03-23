/**
 * \class KDBPublisherCSVLoggerTask
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

#include <chrono>
#include <vector>
#include <array>
#include "CSVLoggerTask.hpp"
#include "KDBPublisher.hpp"

constexpr auto STOCKNR = 26 * 26 * 26 * 26;

class KDBPublisherCSVLoggerTask: public CSVLoggerTask, public KDBPublisher {
    protected:  
        std::vector<std::array<char, 5>> stockUniverse;
        std::vector<char*> stockToSend;        

    public: 
        KDBPublisherCSVLoggerTask(unsigned long, const char* argv[]);
        bool run(const TIME&, const TIME&);
};

KDBPublisherCSVLoggerTask::KDBPublisherCSVLoggerTask(unsigned long triggerNr, const char* argv[]) 
    : CSVLoggerTask(triggerNr, argv), KDBPublisher(triggerNr, argv+1) {  
    tableName = (ks((S) "trade"));

    std::srand(std::time(nullptr));

    stockUniverse.resize(STOCKNR);
    unsigned int i=0;
    for (char a = 'A'; a <= 'Z'; ++a) {
        for (char b = 'A'; b <= 'Z'; ++b) {
            for (char c = 'A'; c <= 'Z'; ++c) {
                for (char d = 'A'; d <= 'Z'; ++d) {
                    stockUniverse[i][0] = a;
                    stockUniverse[i][1] = b;
                    stockUniverse[i][2] = c;
                    stockUniverse[i][3] = d;
                    stockUniverse[i][4] = '\0';
                    ++i;
                }
            }
        }
    }

    stockToSend.reserve(triggerNr);
    for (i = 0; i< triggerNr; ++i) {
        stockToSend.push_back(stockUniverse[rand() % STOCKNR].data());
    }

}

bool inline KDBPublisherCSVLoggerTask::run(const TIME& expected, const TIME& real) {
    static char stop = 'i';            /* charactor example */
    static int size = 444;            /* long example */
    static double price = 12.12;       /* double example */
    static char condition = 'c'; 

    unsigned long sq = expectedTriggerTimes.size();
    char* const stock = stockToSend[sq];

    K row = knk(7, ks(stock), kj(sq), kc(stop), ki(size), kf(price), kc(condition), 
        ktj(-KP, DURNANO((std::chrono::system_clock::now() - kdb_start).time_since_epoch())));

    bool res = KDBPublisher::sendUpdate<false>(row);

    CSVLoggerTask::run(expected, real);
    return res;
}

