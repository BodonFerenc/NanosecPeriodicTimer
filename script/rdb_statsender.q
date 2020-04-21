collectorHandler: 0i;
registerStatCollector:{[]
  show "Stat collector registration on handler ", string .z.w;
  `collectorHandler set .z.w;
  };

sendStat: {[stat]
  if[0 < collectorHandler; collectorHandler (`collectRDBStat; stat)];
  };
