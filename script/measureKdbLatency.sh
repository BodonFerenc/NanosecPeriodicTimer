#!/usr/bin/env bash

set -eu -o pipefail

if test "$#" -lt 4; then
   echo "Please provide four paramters:
      - output csv file name of the publisher, e.g. out/publisher.csv 
      - frequency, e.g. 5000, 
      - test duration in seconds e.g. 120, 
      - an output file name to store statistics, e.g. stat.csv"
   exit 1
fi

PUBLISHEROUTPUT=$1
FREQ=$2
DURATION=$3
OUTPUT=$4

LOGDIR=../log
mkdir -p $LOGDIR

rdbOutputFileName=/tmp/rdb.csv
echo "Starting RDB script in the background..."
nohup numactl --physcpubind=0 q rdb_latency.q -output $rdbOutputFileName -p 5001 2>&1 > ${LOGDIR}/rdb.txt &
RDB_PID=$!

nohup perf stat -x " " -p $RDB_PID -e task-clock --log-fd 1 > ${LOGDIR}/perf.txt &
PERF_PID=$!

sleep 1

echo

echo "Starting publisher with frequency $FREQ for duration $DURATION"
if [ -z ${USETCP+x} ]; 
then 
  host=0.0.0.0
  echo "using unix socket"
else
  host=localhost
  echo "using TCP/IP"
fi
numactl --physcpubind=1 ../bin/KDBPublishLatencyTester $FREQ $DURATION $PUBLISHEROUTPUT $host 5001

tempOutputFileName=/tmp/stat.csv
echo
echo "Processing output $PUBLISHEROUTPUT"
q generatePublisherLatencyStats.q $PUBLISHEROUTPUT $tempOutputFileName -q

echo "Waiting for perf stat (PID: $PERF_PID) and RDB (PID: $RDB_PID) to finish"
wait $RDB_PID
wait $PERF_PID

RDBCPUUSAGE=($(cat ${LOGDIR}/perf.txt))
echo "RDB CPU usage ${RDBCPUUSAGE[5]}"

METAFILE=/tmp/rdbperfmeta.csv

echo "Creating csv for the meta data..."
echo "frequency,duration,RDB CPU Usage" > $METAFILE
echo "$FREQ,$DURATION,${RDBCPUUSAGE[5]}" >> $METAFILE

echo "Merging meta data with publisher and RDB statistics"
paste -d, $METAFILE $tempOutputFileName $rdbOutputFileName > $OUTPUT

exit 0