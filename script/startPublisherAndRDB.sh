#!/usr/bin/env bash

function log () {
    (( VERBOSE )) && echo "$@"
}


LOGDIR=../log
mkdir -p $LOGDIR

RDBSCRIPTFULL="${RDBSCRIPT} $GROUPEDOPT -q -p $RDBPORT"
if [[ $RDBHOST == 0.0.0.0 || $RDBHOST == localhost ]]; then
    ISLOCAL=true
    log "Starting RDB script in the background..."
    nohup numactl --physcpubind=0 q ${RDBSCRIPTFULL} > ${LOGDIR}/rdb.txt 2>&1 &
    RDB_PID=$!
else 
    ISLOCAL=false
    SCRIPTDIR=$(pwd)
    nohup ssh -o "StrictHostKeyChecking no" $RDBHOST "cd ${SCRIPTDIR}; nice -n 19 numactl --physcpubind=0 ${QHOME}/l64/q ${RDBSCRIPTFULL}" > ${LOGDIR}/rdb.txt 2>&1 &
fi

sleep 1   # wait a sec till RDB comes up. TODO: implement more robust solution

log "Starting publisher with frequency $FREQ for duration $DURATION"
nohup numactl --physcpubind=1 ../bin/KDBPublishLatencyTester $FREQ $DURATION \
    $TIMEROUTPUTFILE $RDBHOST $RDBPORT $FLUSH $TIMERSTATONLY $BATCHSIZE $BATCHTYPE > ${LOGDIR}/publisher.txt 2>&1 &
PUB_PID=$!

afterStartWork

if [[ $ISLOCAL == true ]]; then
    nohup perf stat -x " " -p $RDB_PID -e task-clock --log-fd 1 > ${LOGDIR}/perf.txt 2>&1 &
    PERF_PID=$!
fi

wait $PUB_PID

if [[ $ISLOCAL == true ]]; then
    wait $RDB_PID
    if wait $PERF_PID; then
        log "Waiting for perf stat (PID: $PERF_PID) and to finish"    
        RDBCPUUSAGESTAT=($(cat ${LOGDIR}/perf.txt))
        RDBCPUUSAGE=${RDBCPUUSAGESTAT[5]}
        log "RDB CPU usage $RDBCPUUSAGE"
    else
        # TODO: invetigate perf stat errors
        log "Error measuring RDB CPU usage, command perf failed."
        RDBCPUUSAGE=''
    fi
else
    RDBCPUUSAGE=''
fi

METAFILE=/tmp/rdbperfmeta.csv

log "Creating csv for the meta data..."
echo "frequency,duration,batchsize,RDB CPU Usage" > $METAFILE
echo "$FREQ,$DURATION,$BATCHSIZE,$RDBCPUUSAGE" >> $METAFILE
