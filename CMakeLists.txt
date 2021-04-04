cmake_minimum_required(VERSION 3.10)

project(NanoSecPeriodicTimer VERSION 1.0
        DESCRIPTION "A single-threaded periodic timer with nanosecond accuracy"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
endif()

add_subdirectory(src)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

enable_testing()
add_test(NAME TESTTIMERONLY COMMAND PeriodicTimerDemo bytimerjumpforward 2000 5 timer.csv)