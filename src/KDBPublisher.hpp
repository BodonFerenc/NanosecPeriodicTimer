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

namespace detail {
        template <typename R1, typename R2>
        using ratio_multiply = decltype(std::ratio_multiply<R1, R2>{});
    }
constexpr std::chrono::duration<int, detail::ratio_multiply<std::ratio<24>, std::chrono::hours::period>> kdb_start(10957);   // number of days between 2000.01.01 and 1970.01.01

#include "Task.hpp"


class KDBPublisher: public Task {
    protected:  
        int socket;

    public: 
        KDBPublisher(unsigned long, const char* argv[]);
        bool run(const TIME&, const TIME&) = 0;
        ~KDBPublisher();
};
