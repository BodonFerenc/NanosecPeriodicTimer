FROM rikorose/gcc-cmake

WORKDIR /usr/src/kdbIngestTester

COPY src src
COPY CMakeLists.txt .


RUN mkdir build; \
    cd build; \
    cmake -D CMAKE_RUNTIME_OUTPUT_DIRECTORY=/usr/local/bin ..; \
    cmake --build .;
