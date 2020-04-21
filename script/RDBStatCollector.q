
cmd: .Q.opt .z.x

show "opening connection to address: ", first cmd[`rdb];
h: hopen hsym `$first cmd[`rdb];
h (`registerStatCollector; ::);
output: first `$cmd[`output];

collectRDBStat: {[stat]
  show "collecting and saving RDB statistics";
  output 0:","0: stat;
  };

.z.pc: {
  show "exitting...";
  exit 0;
  };