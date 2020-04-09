\l schema.q

cmd: .Q.opt .z.x
if [not `output in key cmd; 
  show "Please provide an output file name by command line parameter -output";
  exit 1];

if [`grouped in key cmd;
	show "setting grouped attribute on column sym";
	update `g#sym from `trade;
	];

output: first `$cmd[`output];

timesAfterInsert: ();

.u.upd: {[t; l] 
  t insert l;
  `timesAfterInsert upsert .z.p};

.z.pc: {
  if[0=x; :()];
  
  rdbDur: .z.p - first timesAfterInsert;

  // extra complexity is required for batch updates
  update kobjcreation: last[time] - first time by batchnr from `trade;
  update adaptertime: first time by batchnr from `trade;  
  update latency: timesAfterInsert[batchnr] - adaptertime from `trade;    

  show "saving latency statistics of table of size ",  string count trade;
  output 0:","0:select 
    RDBduration: enlist rdbDur % 1000 * 1000 * 1000,
    recRowNr: count i,
    medKobjCreation: med kobjcreation,
    maxLatency: `long$max latency,
    minLatency: `long$min latency, 
    avgLatency: avg latency,
    medLatency: `long$med latency from trade;     // force to long for easier post-processing

  show "exitting...";
  exit 0
  };
