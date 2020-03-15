#pragma once

#include <chrono>
#include <functional>
#include <thread>
#include <type_traits>

#include <cstdint>

namespace TimerRunner
{
    // extending chrono for you use case
    // days from HH's date: https://github.com/HowardHinnant/date
    namespace detail {
        template <typename R1, typename R2>
        using ratio_multiply = decltype(std::ratio_multiply<R1, R2>{});
    }
    using days = std::chrono::duration<int, detail::ratio_multiply<std::ratio<24>, std::chrono::hours::period>>;
    // accept system time with any duration: see C++20
    template <class Rep, class Period>
    using sys_time = std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<Rep, Period>>;
    // 2000.01.01 - 1970.01.01 = 10957 days
    constexpr days kdb_start{ 10957 };

    // sleep by OS
    template <class Rep, class Period, class Func>
    void OSPeriodicRunner(const std::chrono::duration<Rep, Period>& dur_, Func fn_)
    {
        while (true) {
            fn_();
            std::this_thread::sleep_for(dur_);
        }
    }

    // 100% CPU: fire callback ASAP time expires
    template <class Rep, class Period, class Func>
    void HFPeriodicRunner(const std::chrono::duration<Rep, Period>& dur_, Func fn_)
    {
        while (true) {
            auto now = std::chrono::steady_clock::now();
            if (now - tp >= dur_) {
                fn_();
                tp = now;
            }
        }
    }

    // microseconds from 2000.01.01 UTC
    template <class Rep, class Period>
    std::int64_t getKDBTime(const sys_time<Rep, Period>& tp_)
    {
        using KDBDuration = std::chrono::microseconds;
        sys_time<Rep, Period> kdbtime{ tp_ };
        kdbtime -= kdb_start;
        return std::chrono::time_point_cast<KDBDuration>(tp_).time_since_epoch().count();
    }
}