FROM rikorose/gcc-cmake

RUN apt-get update \
    && apt-get -yy --option=Dpkg::options::=--force-unsafe-io --no-install-recommends install \
        rlwrap \
    && apt-get clean \
	&& find /var/lib/apt/lists -type f -delete

RUN echo '#!/bin/bash\nrlwrap -pPURPLE -r -c -i /q/l64/q "$@"' > /usr/bin/q && \
    chmod +x /usr/bin/q

WORKDIR /usr/src/kdbIngestTester

COPY src src
COPY script script
COPY CMakeLists.txt .


RUN mkdir build; \
    cd build; \
    cmake -D CMAKE_RUNTIME_OUTPUT_DIRECTORY=/usr/local/bin ..; \
    cmake --build .;
