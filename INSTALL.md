## Building the binaries

You need to have `cmake` and a c++ compiler with `c++14` support installed. To build the binary do

```bash
$ cmake -E make_directory build
$ cd build
$ cmake ..
$ cmake --build .
```

The binaries are built in directory `bin`.

## Using a Docker image

If you don't want to bother with building the binary, then you can grab a docker image that comes with `gcc` an `cmake`.

```bash
$ docker pull ferencbodon/kdb_ingest_tester:1
```

You need [sudo](https://en.wikipedia.org/wiki/Sudo) if you run docker commands on Linux. If you are tired of typing `sudo` then you can [add yourself to the group `docker`](https://docs.docker.com/engine/install/linux-postinstall/).
