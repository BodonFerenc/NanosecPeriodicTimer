# High precision, high frequency periodic timer demo

This small repository contains C files that demonstrate the two ways of creating a periodic timer. The first approach is based on sleep the second is based constantly checking the time to see when to trigger.

To build the two binaries:
```
$ make
```

To run the executables:
```
$ ./bin/PeriodicTimerDemo bytimerjumpforward 200000 30 res.csv
```
which will run a time check based periodic timer for 30 seconds. The frequency of triggers is 20000, i.e. 20000 events per second and the planned and actual trigger times are saved in file `res.csv`. Pass `bysleep` as first parameter for a sleep based timer.

The program will output a few useful information, e.g the planned delays between triggers and the average of actual and planned delays. You can get a full distribution of the delays, .e.g by a simple q process:

```
$ q
q) t: ("JJJ";enlist",") 0:hsym `res.csv
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
It is recommended to have a CPU with constant, invariant TSC (time-stamp counter) tha also support from RDTSCP instructions. This allows fast retrievel of the time. The server I worked on, function [clock_gettime](https://linux.die.net/man/3/clock_gettime) executes in 26 nanosec. Run binary `bin/gettimeMeter` to measure execution time of getting the time.

You can display clock support this via Linux command

```
$ cat /proc/cpuinfo | grep -i tsc
flags : ... tsc  rdtscp constant_tsc nonstop_tsc ...
```
If you CPU does not support constant TSC (that keeps all TSC’s synchronized across all cores) then you need to bind the application to a CPU by

```
$ taskset -c 0 ...
```

The program is using clock CLOCK_MONOTONIC and you get similar results with CLOCK_REALTIME.

Read this [excellent article](http://btorpey.github.io/blog/2014/02/18/clock-sources-in-linux/) for more details on clock support by the Linux kernel.
