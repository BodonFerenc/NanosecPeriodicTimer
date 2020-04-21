collectorHandler: 0i;
registerStatCollector:{[]
  show "Stat collector registration on handler ", string .z.w;
  `collectorHandler set .z.w;
  };

sendStat: {[stat]
    collectorHandler (`collectRDBStat; stat);
  };
