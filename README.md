# High precision, high frequency periodic timer demo

This small repository contains C files that demonstrate the two ways of creating a periodic timer. The first approach is based on sleep the second is based constantly checking the time to see when to trigger.

To build the two binaries:
```
$ make
```

To run the executables:
```
$ ./bin/PeriodicTimerDemo bytimerjumpforward 200000 30 timer.csv
```
which will run a time check based periodic timer for 30 seconds. The frequency of triggers is 20000, i.e. 20000 events per second and the planned and actual trigger times are saved in file `timer.csv`. Pass `bysleep` as first parameter for a sleep based timer.

The program will output a few useful information, e.g the planned delays between triggers and the average of actual and planned delays. You can get a full distribution of the delays, .e.g by a simple q process:

```
$ q
q) t: ("JJJ";enlist",") 0:hsym `timer.csv
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

```
$ q script/generatePublisherLatencyStats.q timer.csv timerStatistics.csv
```

## Clock aspects
It is recommended to have a CPU with constant, invariant TSC (time-stamp counter) tha also support from RDTSCP instructions. This allows fast retrievel of the time. The Linux server I worked on, function [chrono::steady_clock::now](https://en.cppreference.com/w/cpp/chrono/steady_clock/now) executes in 26 nanosec. Run binary `bin/gettimeMeter` to measure execution time of getting the time.

You can display clock support this via Linux command

```
$ cat /proc/cpuinfo | grep -i tsc
flags : ... tsc  rdtscp constant_tsc nonstop_tsc ...
```
If you CPU does not support constant TSC (that keeps all TSC’s synchronized across all cores) then you need to bind the application to a CPU by `taskset` or `numactl`

```
$ taskset -c 0 ...
```

The program is using clock CLOCK_MONOTONIC and you get similar results with CLOCK_REALTIME.

Read this [excellent article](http://btorpey.github.io/blog/2014/02/18/clock-sources-in-linux/) for more details on clock support by the Linux kernel.

## kdb+ trade table publisher
Class [KDBTradePublisher](https://github.com/BodonFerenc/NanosecPeriodicTimer/blob/master/src/KDBTradePublisher.cpp) sends single row updates of table [trade](https://github.com/BodonFerenc/NanosecPeriodicTimer/blob/master/q/schema.q) to a [kdb+ process](https://github.com/BodonFerenc/NanosecPeriodicTimer/blob/master/q/rdb_light.q) when the timer triggers. [kdb+ needs to be installed](https://code.kx.com/q/learn/).
To run do the following after command `make`

```
# In Terminal 1:
$ cd script
$ q rdb_light.q -p 5003

q) tradeTP
sym time price size stop ex
---------------------------

# In Terminal 2:
./bin/PeriodicKDBPublisher 10000 20 localhost 5003
```

Now switch back to `Terminal 1` and check the content of table trade or see how its size grows by executing command `count tradeTP` in the q interpreter.

## Measuring kdb+ ingest latency
The script can also be used to measure how long it takes from a trigger event, through a trade data publish till it arrives into a kdb+ process that inserts the data into its local table. 

```
# In Terminal 1:
$ cd script
$ q rdb_latency.q -output ../statistics.csv -p 5003

q) trade
sym time price size stop ex
---------------------------

# In Terminal 2:
./bin/KDBPublishLatencyTester 10000 20 ../timerStat.csv localhost 5003 0
```

If the publisher and the kdb+ process are on the same machine then you can unix sockets. All you need to do is changing the host parameter to `0.0.0.0`. This will result in lower data transfer latencies.

```
# In Terminal 2:
./bin/KDBPublishLatencyTester 10000 20 ../timerStat.csv 0.0.0.0 5003 0
```


You can observe the latency statistics in file `../statistics.csv`. If you would like to see all statistics in a single view then you can simply merge publisher's and RDB's output by Linux command [paste](https://en.wikipedia.org/wiki/Paste_(Unix))

```
paste -d, ../timerStat.csv ../statistics.csv
```

If you dont want to do all these manually then you can use bash script `measureKdbLatency.sh`. It starts RDB and publisher for you and even measures RDB CPU usage rate.

```
./measureKdbLatency.sh --freq 10000 --dur 20 --output ../statistics.csv --rdbhost localhost
```

Use `--rdbhost localhost` if you would like to use TCP/IP connection and `--flush` to [flush output buffer](https://code.kx.com/q/basics/ipc/#block-queue-flush) after each send message.

Script `measureKdbLatency.sh` also supports **remote kdb+ process via TCP**. If you pass an IP address via the `--rdbhost` parameter then the script will start the server via ssh. You probably want to use `~/.ssh/config` to provide the user name and private key location for the remote server. The RDB writes out latency statistics that your local host needs so you need a filesystem that is available from both boxes. By default the RDB output is written to `/tmp/rdb.csv` but you can overwrite this with environment variable `RDBOUTPUTFILE` like

```
RDBOUTPUTFILE=/nfs/data/rdb.csv ./measureKdbLatency.sh --freq 10000 --dur 20 --output ../statistics.csv --rdbhost 72.7.9.248
```

You might want to figure out the maximal frequency of updates your kdb+ process can ingest. You can manually run `measureKdbLatency.sh` with various parameters or use `./measureMultipleKdbLatency.sh` that does this for you and create a summary table.

```
./measureMultipleKdbLatency.sh --freq 10000,50000,100000 --dur 60,180 --outputdir ../out
```

The file that collects all statistics will be placed at `../out/summary.csv`.


### prerequisite
The following packages are required by script `measureKdbLatency.sh`
   * numactl:. CentOS install: `sudo yum install numactl`
   * perf tools: `sudo yum install perf gawk`
