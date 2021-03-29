# High precision, high frequency periodic timer demo

This small repository contains C++ files that demonstrate the two ways of creating a single threaded periodic timer. We can use this timer e.g. to benchmark kdb+ ingestion latencies and throughput. The first approach is based on sleep the second is based constantly checking the time to see when to trigger.

## Install
You need to have `cmake` and a c++ compiler with `c++14` support installed. To build the binary do

```bash
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

The binaries are built in directory `bin`.

If you don't want to bother with building the binary, then you can grab a docker image that comes with `gcc` an `cmake`.

```bash
$ docker pull ferencbodon/kdb_ingest_tester:1
```

You need [sudo](https://en.wikipedia.org/wiki/Sudo) if you run docker commands on Linux.

## Running the executables
Command

```bash
$ mkdir -p out
$ ./bin/PeriodicTimerDemo bytimerjumpforward 200000 30 out/timer.csv
```

or with docker

```bash
$ docker run -v "$(pwd)/out:/tmp" --rm -it ferencbodon/kdb_ingest_tester:1 PeriodicTimerDemo bytimerjumpforward 200000 30 /tmp/timer.csv
```

runs a time check based periodic timer for 30 seconds. The frequency of triggers is 20000, i.e. 20000 events per second and the planned and actual trigger times are saved in file `out/timer.csv`. Pass `bysleep` as first parameter for a sleep based timer.

The program will output a few useful information, e.g the planned delays between triggers and the average of actual and planned delays. You can get a full distribution of the delays, .e.g by a simple q process:

```
$ q
q) t: ("JJJ";enlist",") 0:hsym `$"out/timer.csv"
q)select median_latency: med latency, avg_latency: avg latency, max_latency: max latency from t
median_latency avg_latency max_latency
--------------------------------------
12             145.4046    428664
q) update rate: nr % sum nr from select nr: count i by 1000 xbar latency from t
latency| nr      rate
-------| --------------------
0      | 5996167 0.9993612
1000   | 25      4.166667e-06
2000   | 21      3.5e-06
3000   | 20      3.333333e-06
4000   | 18      3e-06
...
```

The q script `generatePublisherLatencyStats.q` also generates some useful statistics (like median of the latencies) in a CSV file specified by its second parameter.

```bash
$ q script/generatePublisherLatencyStats.q out/timer.csv out/timerStatistics.csv
```

## Clock aspects
It is recommended to have a CPU with constant, invariant TSC (time-stamp counter) that also support from RDTSCP instructions. This allows fast retrieval of the time. The Linux server I worked on, function [chrono::steady_clock::now](https://en.cppreference.com/w/cpp/chrono/steady_clock/now) executes in 26 nanoseconds. Run binary `bin/gettimeMeter` to measure execution time of getting the time.

You can display clock support this via Linux command

```bash
$ cat /proc/cpuinfo | grep -i tsc
flags : ... tsc  rdtscp constant_tsc nonstop_tsc ...
```
If you CPU does not support constant TSC (that keeps all TSC’s synchronized across all cores) then you need to bind the application to a CPU by `taskset` or `numactl`

```bash
$ taskset -c 0 ...
```

The program is using clock CLOCK_MONOTONIC and you get similar results with CLOCK_REALTIME.

Read this [excellent article](http://btorpey.github.io/blog/2014/02/18/clock-sources-in-linux/) for more details on clock support by the Linux kernel.

## kdb+ trade table publisher
Class [KDBTradePublisher](https://github.com/BodonFerenc/NanosecPeriodicTimer/blob/master/src/KDBTradePublisher.cpp) sends single row updates of table [trade](https://github.com/BodonFerenc/NanosecPeriodicTimer/blob/master/q/schema.q) to a [kdb+ process](https://github.com/BodonFerenc/NanosecPeriodicTimer/blob/master/q/rdb_light.q) when the timer triggers. [kdb+ needs to be installed](https://code.kx.com/q/learn/).

To run do the following after command `make`

```bash
# In Terminal 1:
$ cd script
$ q rdb_light.q -p 5003

q) tradeTP
sym time price size stop ex
---------------------------

# In Terminal 2:
./bin/PeriodicKDBPublisher 10000 20 localhost 5003
```

If you are on Mac or Windows and have the C++ publisher in a docker container, then you can use magic hostname `host.docker.internal` as per

```bash
$ docker run --rm -it ferencbodon/kdb_ingest_tester:1 PeriodicKDBPublisher 10000 20 host.docker.internal 5003
```

On Linux, you need switch `--net=host`

```bash
$ sudo docker run --net=host --rm -it ferencbodon/kdb_ingest_tester:1 PeriodicKDBPublisher 10000 20 localhost 5003
```

Now switch back to `Terminal 1` and check the content of table trade or see how its size grows by executing command `count tradeTP` in the q interpreter.

## Measuring kdb+ ingest latency
The script can also be used to measure how long it takes from a trigger event, through a trade data publish till it arrives into a kdb+ process that inserts the data into its local table. Script `rdb_latency.q` differs from `rdb_light` in storing a timestamp for each update. This timestamp can be compared to the real trigger time sent by the timer to obtain _ingest latency_.

```bash
# In Terminal 1:
$ cd script
$ q rdb_latency.q -output ../out/statistics.csv -p 5003

q) trade
sym time price size stop ex
---------------------------

# In Terminal 2:
./bin/KDBPublishLatencyTester 10000 20 ../out/timerStat.csv localhost 5003 -s
```

The output CSV contains the following fields:
   * `realFrequency`: The number of messages sent divided by the timer duration (approx. difference of first and last trigger time).
   * `sentMessageNr`: The number of messages sent.
   * `maxTimerLatency`: The maximum of the timer latency, which is the difference of the planned and real trigger time.
   * `avgTimerLatency`: The average timer latency.
   * `medTimerLatency`: The median timer latency.

If the publisher and the kdb+ process are on the same machine then you can unix sockets. All you need to do is changing the host parameter to `0.0.0.0`. This will result in lower data transfer latencies.

```bash
# In Terminal 2:
./bin/KDBPublishLatencyTester 10000 20 ../out/timerStat.csv 0.0.0.0 5003 -s
```

You can observe the latency statistics in file `../out/statistics.csv`. The CSV contains four statistic of the ingest latency, the maximum (_maxLatency_), the minimum (_minLatency_), the average (_avgLatency_) and the median (_medLatency_) and some core statistics:
   * `RDBduration`: The difference between the first update's time and the time of publisher disconnect (observed via [.z.pc](https://code.kx.com/q/ref/dotz/#zpc-close))
   * `recMessageNr`: Number of message received.
   * `recRowNr`: Number of rows received. This number differs from _recMessageNr_ if publisher sends batch updates.
   * `medKobjCreation`: This metric is interesting for batch updates and shows the median of the time differences of the first and the last rows of each batch. You can learn how long it takes to create the K objects in the publisher.

If you would like to see all statistics in a single view then you can simply merge publisher's and RDB's output by Linux command [paste](https://en.wikipedia.org/wiki/Paste_(Unix))

```bash
$ paste -d, ../out/timerStat.csv ../out/statistics.csv
```

If the RDB is on a remote host and there is no shared filesystem (e.g. NFS) between the hosts then you can use kdb+ script `RDBStatCollector.q` on the publisher's host to fetch statistics from the RDB.

```bash
$ q RDBStatCollector.q -rdb 72.7.9.248 -output ../out/statistics.csv
```

You don't need to start processes manually and merge the outputs, bash script `measureKdbLatency.sh` does it for you. It starts an RDB, RDB statistics fetcher and a publisher for you and even measures RDB CPU usage rate.

```bash
$ ./measureKdbLatency.sh --freq 10000 --dur 20 --output ../out/statistics.csv --rdbhost localhost
```

Use `--rdbhost localhost` if you would like to use TCP/IP connection and `--flush` to [flush output buffer](https://code.kx.com/q/basics/ipc/#block-queue-flush) after each send message. This starts the timer with `-f` command line parameter.

Script `measureKdbLatency.sh` also supports **remote kdb+ process via TCP**. If you pass an IP address via the `--rdbhost` parameter then the script will start the RDB on remote host via ssh. You probably want to use `~/.ssh/config` to provide the user name and private key location for the remote server.

The C++ publisher and the bash scripts handle batch update. Use parameter `batchsize` to specify batch size. You can decide if each trigger should generate a batch of rows (`--batchtype batch`) or each trigger should only generate a single row, that the publisher caches and if the size of the cache reaches a limit then it sends the batch to the kdb+ process. This option requires command line parameter `--batchtype cache`. The bash script passes batch parameters to the C++ publisher via `-b` and `-t` command line parameters, e.g. `-t cache -b 100`.

You might want to figure out the maximal frequency of updates your kdb+ process can ingest. You can manually run `measureKdbLatency.sh` with various parameters or use `./measureMultipleKdbLatency.sh` that does this for you and create a summary table.

```bash
./measureMultipleKdbLatency.sh --freq 10000,50000,100000 --dur 60,180 --outputdir ../out
```

The file that collects all statistics will be placed at `../out/summary.csv`.


### prerequisite
The following packages are required by script `measureKdbLatency.sh`
   * numactl:. CentOS install: `sudo yum install numactl`
   * perf tools: `sudo yum install perf gawk`
