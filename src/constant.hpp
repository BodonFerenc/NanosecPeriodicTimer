#pragma once

#include <chrono>

constexpr auto BILLION=1000000000l;

// what you really want here is monotonic clock
using TIME = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

template <typename... Args>
auto DURNANO(Args&&... args) -> auto {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(std::forward<Args>(args)...).count();
}

