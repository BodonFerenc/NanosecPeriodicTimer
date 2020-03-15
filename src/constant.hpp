#pragma once

#include <chrono>

// what you really want here is HH's date::days or C++20 std::days: let the compiler do the job:
constexpr std::days kdb_start{ 10957 }; // 2000.01.01 - 1970.01.01

// what you really want here is monotonic clock
using TIME = std::chrono::time_point<std::chrono::steady_clock>;

// you don't really want it: keep type safety
template <typename... Args>
auto DURNANO(Args&&... args) -> unsigned long {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(std::forward<Args>(args)...).count();
}

