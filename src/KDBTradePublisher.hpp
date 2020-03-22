/**
 * \class KDBTradePublisher
 *
 *
 * \brief Task that sends a trade update to kdb+
 *
 * During the construction it initializes a connection to a kdb+ process
 * The symbol column is pregenerated to demostrate how to improve performance by pregenerating data.
 * This better represent real-life feed handlers.
 * Some fields are generated when the timer triggers. This might be useful for e.g kdb+ plant testing.
 *
 * \author Ferenc Bodon
 *
 */

#pragma once

#include <vector>
#include <array>
#include "KDBPublisher.hpp"


class KDBTradePublisher: public KDBPublisher {
    private:  
        std::vector<std::array<char, 5>> stockUniverse;
        std::vector<char*> stockToSend;        
        std::vector<char*>::iterator stock_it;

    public: 
        KDBTradePublisher(unsigned long, const char* argv[]);
        bool run(const TIME&, const TIME&);
};
