\l log.q

isOutputFileSet: 0b;
if [`output in key cmd;
  output: first `$cmd[`output];
  isOutputFileSet: 1b;
	];

collectorHandler: 0i;
registerStatCollector:{[]
  LOG "Stat collector registration on handler ", string .z.w;
  `collectorHandler set .z.w;
  };

sendStat: {[stat]
  start: .z.p;

  if[0 < collectorHandler;
    LOG "sending RDB statistics to stat collector process ";
    collectorHandler (`collectRDBStat; stat)];

  if[isOutputFileSet;
      LOG "saving RDB statistics to ", string output;
      output 0:","0: stat];

  end: .z.p;
  LOG "Time required for handling RDB statistics: ", string end - start;
  };
