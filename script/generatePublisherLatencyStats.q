timerOutput: hsym `$first .z.x;
outputFile: hsym `$last .z.x;

outputFile 0:csv 0:select realFrequency: 1000 * 1000 * 1000 * count[i] % last[real] - first real,
             maxTimerLatency: `long$max latency, 
             avgTimerLatency: avg latency,
             medTimerLatency: med latency 
             from ("JJJ";enlist",") 0:timerOutput;

exit 0;