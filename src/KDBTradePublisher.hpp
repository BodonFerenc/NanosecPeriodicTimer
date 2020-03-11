/**
 * \class KDBTradePublisher
 *
 *
 * \brief Task that sends a trade update to kdb+
 *
 * During the construction it initializes a connection to a kdb+ process
 * The symbol column is pregenerated to demostrate how to improve performance by pregenerating data.
 * Some fields are generated when the timer triggers.
 *
 * \author Ferenc Bodon
 *
 */

#pragma once

#include <vector>
#include <array>
#include "Task.hpp"


class KDBTradePublisher: public Task {
    private:  
        int socket;
        unsigned long counter;
        vector<array<char, 5>> stockUniverse;
        vector<char*> stockToSend;        

    public: 
        KDBTradePublisher(unsigned long, const char* argv[]);
        bool run(const TIME&, const TIME&);
        ~KDBTradePublisher();
};
