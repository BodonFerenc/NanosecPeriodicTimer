input: `$first .z.x;
output: `$last .z.x; 

t: `frequency`batchsize xasc ("JJJFJJFJJJJFJ";enlist",") 0:hsym input;
update largestBatchSize: max batchsize by frequency from `t;
bestMedLatency: select 
    largestBatchSize: last batchsize, 
    medLatencyAtLargestBatchSize: last medLatency by frequency 
        from t where batchsize=(max; batchsize) fby frequency;

output 0:","0:t lj bestMedLatency;
exit 0;