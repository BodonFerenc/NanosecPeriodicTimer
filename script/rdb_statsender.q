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

  $[0 < collectorHandler; [
    if[isOutputFileSet; show "Parameter output is ignored, using a stat collector process"];
    collectorHandler (`collectRDBStat; stat)
    ];
    $[isOutputFileSet; [
      show "saving RDB statistics to ", string output;    
      output 0:","0: stat;
      ];
      show stat;
    ]];

  end: .z.p;
  show "Time required for handling RDB statistics: ", string end - start;
  };
