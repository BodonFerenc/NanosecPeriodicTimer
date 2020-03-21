/**
 * \class KDBPublisher
 *
 *
 * \brief Task that sends a update to kdb+
 *
 * During the construction it initializes a connection to a kdb+ process
 * and in destructor it closes the connection.
 *
 * \author Ferenc Bodon
 *
 */

#pragma once

#include "Task.hpp"


class KDBPublisher: public Task {
    protected:  
        int socket;

    public: 
        KDBPublisher(unsigned long, const char* argv[]);
        bool run(const TIME&, const TIME&) = 0;
        ~KDBPublisher();
};
