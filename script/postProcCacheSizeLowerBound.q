input: `$first .z.x;
output: `$last .z.x;

\l utils/csvutil.q

t: `frequency`batchsize`medLatency`isStable#.csv.read[input];

res: t lj `frequency xkey select frequency,
    minBatchSize: batchsize,
    medLatencyAtMinBatchSize: medLatency from
    t where isStable, batchsize=(min; batchsize) fby frequency;

output 0:","0:flip -2#flip t lj res;
exit 0;