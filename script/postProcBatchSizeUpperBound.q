input: `$first .z.x;
output: `$last .z.x;

\l utils/csvutil.q

t: `frequency`batchsize`medLatency`isStable#.csv.read[input];

res: t lj `frequency xkey select frequency,
    maxBatchSize: batchsize,
    medLatencyAtMaxBatchSize: medLatency from
    t where isStable, batchsize=(max; batchsize) fby frequency;

output 0:","0:flip -2#flip res;
exit 0;