#pragma once

#include <chrono>

constexpr auto BILLION=1000000000l;
constexpr unsigned long NUM_NANOSECS_BETWEEN_1970_2000 = 946684800 * BILLION;     // useful for KDB time conversion

// what you really want here is monotonic clock
using TIME = std::chrono::time_point<std::chrono::system_clock>;

// you don't really want it: keep type safety
template <typename... Args>
auto DURNANO(Args&&... args) -> unsigned long {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(std::forward<Args>(args)...).count();
}

