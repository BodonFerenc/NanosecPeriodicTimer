isOutputFileSet: 0b;
if [`output in key cmd;
  output: first `$cmd[`output];
  isOutputFileSet: 1b;
	];

collectorHandler: 0i;
registerStatCollector:{[]
  show "Stat collector registration on handler ", string .z.w;
  `collectorHandler set .z.w;
  };

sendStat: {[stat]
  start: .z.p;

  if[0 < collectorHandler;
    show "sending RDB statistics to stat collector process ";    
    collectorHandler (`collectRDBStat; stat)];

  if[isOutputFileSet;
      show "saving RDB statistics to ", string output;    
      output 0:","0: stat];

  end: .z.p;
  show "Time required for handling RDB statistics: ", string end - start;
  };
