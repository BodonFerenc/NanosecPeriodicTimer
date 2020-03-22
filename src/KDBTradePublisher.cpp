#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include "constant.hpp"
#include "KDBTradePublisher.hpp"
#include "k.h"

constexpr auto STOCKNR = 26 * 26 * 26 * 26;

KDBTradePublisher::KDBTradePublisher(unsigned long triggerNr, const char* argv[]) : 
    KDBPublisher(triggerNr, argv), counter(0) {  
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

}

bool inline KDBTradePublisher::run(const TIME& expected, const TIME& real) {
    char* const stock = stockToSend[counter];
    // price between 100 and 120
    float price = 100.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(120-100)));

    K row = knk(6, ks(stock), ktj(-KP, DURNANO((real - kdb_start).time_since_epoch())),
        kf(price), ki(rand()), kb(rand() % 2), kc('e'));

    K r = k(-socket, (char *) ".u.upd", ks((S) "trade"), row, (K)0);
    ++counter;
         
    /* if network error, async call will return 0 */
    if (!r)
    {
       std::cout << "Network Error populating table" << std::endl;
       return(false);
    }
    return true;
}
