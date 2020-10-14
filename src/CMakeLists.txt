add_executable(PeriodicTimerDemo
    PeriodicTimerDemo.cpp
    CSVLoggerTask.cpp)

add_executable(PeriodicKDBPublisher
    PeriodicKDBPublisher.cpp
    KDBTradePublisher.cpp
    KDBPublisher.cpp
    KDBSymGenerator.cpp
    c.o)

add_executable(KDBPublishLatencyTester
   KDBPublishLatencyTester.cpp
   KDBPublisher.cpp
   CSVLoggerTask.cpp
   CSVStatLoggerTask.cpp
   KDBSymGenerator.cpp
   c.o)

file(DOWNLOAD "https://github.com/KxSystems/kdb/raw/master/c/c/k.h" "${CMAKE_CURRENT_SOURCE_DIR}/k.h" )

if(APPLE)
   set(OSFLAG m64)
elseif(WIN32)
   set(OSFLAG w64)
else()
   set(OSFLAG l64)
endif()
file(DOWNLOAD "https://github.com/KxSystems/kdb/raw/master/${OSFLAG}/c.o" "${CMAKE_CURRENT_BINARY_DIR}/c.o" )

set_target_properties(KDBPublishLatencyTester PROPERTIES
    COMPILE_FLAGS -DKXVER=3
)
target_link_libraries(KDBPublishLatencyTester pthread)  # required by Ubuntu 18.04

set_target_properties(PeriodicKDBPublisher PROPERTIES
    COMPILE_FLAGS -DKXVER=3
)
target_link_libraries(PeriodicKDBPublisher pthread)  # required by Ubuntu 18.04
