input: `$first .z.x;
output: `$last .z.x; 

// TODO: find more robust solution than position based reference!
t: `frequency`batchsize xasc ("J*J************J";enlist",") 0:hsym input;
res: fills `frequency xdesc select largestBatchSize: batchsize 1, 
                                   largestBatchSize: medLatency 1 by frequency from `batchsize xdesc t;

output 0:","0:flip -2#flip t lj res;
exit 0;