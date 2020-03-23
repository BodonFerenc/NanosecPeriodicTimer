#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include "constant.hpp"
#include "KDBTradePublisher.hpp"
#include "k.h"

constexpr auto STOCKNR = 26 * 26 * 26 * 26;

KDBTradePublisher::KDBTradePublisher(unsigned long triggerNr, const char* argv[]) : 
    KDBPublisher(triggerNr, argv) {  
    tableName = (ks((S) "tradeTP"));

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
    srand(time(nullptr));
    stockToSend.reserve(triggerNr);
    for (i = 0; i< triggerNr; ++i) {
        stockToSend.push_back(stockUniverse[rand() % STOCKNR].data());
    }
    stock_it = stockToSend.begin();
}

bool inline KDBTradePublisher::run(const TIME& expected, const TIME& real) {
    // price between 100 and 120
    float price = 100.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(120-100)));

    K row = knk(6, ks(*stock_it), ktj(-KP, DURNANO((real - kdb_start).time_since_epoch())),
        kf(price), ki(rand()), kb(rand() % 2), kc('e'));

    ++stock_it;
    return KDBPublisher::sendUpdate<false>(row);
}
