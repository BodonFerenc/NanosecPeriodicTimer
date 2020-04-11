input: `$first .z.x;
output: `$last .z.x; 

// TODO: find more robust solution than hardcoding all types!
t: `frequency`batchsize xasc ("JJJFJJJJFJFJJJJFJ";enlist",") 0:hsym input;
bestMedLatency: select 
    largestBatchSize: last batchsize, 
    medLatencyAtLargestBatchSize: last medLatency by frequency 
        from t where batchsize=(max; batchsize) fby frequency;

output 0:","0:t lj bestMedLatency;
exit 0;