#include <iostream>
#include <ctime>
#include "constant.hpp"
#include "KDBTradePublisher.hpp"
#include "k.h"

KDBTradePublisher::KDBTradePublisher(unsigned long triggerNr, std::string host, int port) :
    KDBPublisher{triggerNr, host, port}, symGenerator{triggerNr} {
    tableName = ks((S) "tradeTP");
}

bool KDBTradePublisher::run(const TIME& expected, const TIME& real) {
    // price between 100 and 120
    float price = 100.0 + static_cast<float> (rand()) / (static_cast<float> (RAND_MAX/(120-100)));

    K row = knk(6, ks(*symGenerator.sym_it), ktj(-KP, DURNANO((real - kdb_start).time_since_epoch())),
        kf(price), ki(rand()), kb(rand() % 2), kc('e'));

    ++symGenerator.sym_it;
    return sendUpdate(row);
}
