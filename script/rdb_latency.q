cmd: .Q.opt .z.x

\l schema.q
\l rdb_statsender.q
\l log.q

// If true then the RDB exits if a connection closes
// Useful for debugging
EXIT: `exit in key cmd;

if [`grouped in key cmd;
	LOG "setting grouped attribute on column sym";
	update `g#sym from `trade;
	];

timesAfterInsert: ();

.u.upd: {[t; l]
  t insert l;
  `timesAfterInsert upsert .z.p};

.z.pc: {
  if[0=x; :()];
  start: .z.p;
  rdbDur: start - first timesAfterInsert;
  LOG "Upper bound of the RDB insert time is ",  string rdbDur;

  // extra complexity is required for batch updates
  update kobjcreation: last[time] - first time by batchnr from `trade;
  update adaptertime: first time by batchnr from `trade;
  update latency: timesAfterInsert[batchnr] - adaptertime from `trade;

  LOG "Calculating latency statistics of table of size ",  string count trade;
  stat: select
    RDBduration: enlist rdbDur % 1000 * 1000 * 1000,
    recMessageNr: count timesAfterInsert,
    recRowNr: count i,
    medKobjCreation: med kobjcreation,
    maxLatency: `long$max latency,
    minLatency: `long$min latency,
    avgLatency: avg latency,
    medLatency: `long$med latency from trade;     // force to long for easier post-processing

  sendStat[stat];

  if[EXIT;
    LOG "exitting...";
    exit 0];
  };
