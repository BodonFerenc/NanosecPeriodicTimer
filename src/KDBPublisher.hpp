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

#include <chrono>
#include <iostream>
#include "k.h"
#include "constant.hpp"

namespace detail {
        template <typename R1, typename R2>
        using ratio_multiply = decltype(std::ratio_multiply<R1, R2>{});
    }
constexpr std::chrono::duration<int, detail::ratio_multiply<std::ratio<24>, std::chrono::hours::period>> kdb_start(10957);   // number of days between 2000.01.01 and 1970.01.01



class KDBPublisher {
    protected:  
        int socket;
        K tableName;

    public: 
        KDBPublisher(unsigned long, const char* argv[]);
        
        template<bool FLUSH=false>
        bool sendUpdate(K row);

        ~KDBPublisher();
};

template<bool FLUSH>
inline bool KDBPublisher::sendUpdate(K row) {
    K r = k(-socket, (char *) ".u.upd", r1(tableName), row, (K)0);
    if (FLUSH) k(-socket, (char *) "[]", (K)0);
         
    /* if network error, async call will return 0 */
    if (!r) {
       std::cout << "Network Error populating table" << std::endl;
       return false;
    }
    return true;
}
