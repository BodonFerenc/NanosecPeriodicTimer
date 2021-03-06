input: `$first .z.x;
output: `$last .z.x;

\l utils/csvutil.q

if[not `basicread in key .csv;
    '"latest csvutil.q is required (released after 2020.05.17)"];

t: `frequency`batchsize`medLatency`isStable#.csv.basicread[input];

res: t lj `frequency xkey select frequency,
    minBatchSize: batchsize,
    medLatencyAtMinBatchSize: medLatency from
    t where isStable, batchsize=(min; batchsize) fby frequency;

output 0:","0:res;
exit 0;