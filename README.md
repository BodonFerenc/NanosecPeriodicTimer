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

## measuring kdb+ latency
The script can also be used to measure how long it takes from a trigger event, through a trade data publish till it arrives into a kdb+ process that inserts the data into its local table. 

```
# In Terminal 1:
$ cd script
$ q rdb_latency.q -output statistics.csv -p 5003

q) trade
sym time price size stop ex
---------------------------

# In Terminal 2:
./bin/KDBPublishLatencyTester 10000 20 timer.csv localhost 5003
```

You can observe the latency statistics in file statistics.csv. 
