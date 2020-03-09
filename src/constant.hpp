#ifndef CONSTANT_H
#define CONSTANT_H

#include <chrono>

constexpr auto BILLION=1000000000l;
constexpr unsigned long NUM_NANOSECS_BETWEEN_1970_2000 = 946684800 * BILLION;     // useful for KDB time conversion

using TIME = std::chrono::time_point<std::chrono::system_clock>;

template <typename... Args>
auto DURNANO(Args&&... args) -> unsigned long {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(std::forward<Args>(args)...).count();
}

#endif
