\l log.q

cmd: .Q.opt .z.x

LOG "opening connection to address: ", first cmd[`rdb];
h: hopen hsym `$first cmd[`rdb];
h (`registerStatCollector; ::);
output: first `$cmd[`output];
STAT: ();

collectRDBStat: {[stat]
  LOG "collecting RDB statistics";
  STAT:: stat;
  };

.z.pc: {
  if[0=x; :()];
  LOG "Saving RDB statistics";
  output 0:","0: STAT;
  LOG "exitting...";
  exit 0;
  };