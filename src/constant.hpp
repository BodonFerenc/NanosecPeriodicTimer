#ifndef CONSTANT_H
#define CONSTANT_H

#include <chrono>

constexpr auto BILLION=1000000000l;
using TIME = std::chrono::time_point<std::chrono::steady_clock>;

template <typename... Args>
auto DURNANO(Args&&... args) -> unsigned long {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(std::forward<Args>(args)...).count();
}

#endif
