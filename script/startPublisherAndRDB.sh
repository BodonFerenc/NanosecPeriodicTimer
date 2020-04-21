#!/usr/bin/env bash

function log () {
    (( VERBOSE )) && echo "$@"
}


LOGDIR=../log
mkdir -p $LOGDIR

RDBSCRIPTFULL="${RDBPRECOMMAND} ${QHOME}/${PLATFORM}/q ${RDBSCRIPT} $GROUPEDOPT -q -p $RDBPORT"
if [[ $RDBHOST == 0.0.0.0 || $RDBHOST == localhost ]]; then
    ISLOCAL=true
    log "Starting RDB script in the background..."
    nohup ${RDBSCRIPTFULL} > ${LOGDIR}/rdb.txt 2>&1 &
    RDB_PID=$!
else 
    ISLOCAL=false
    SCRIPTDIR=$(pwd)
    nohup ssh -o "StrictHostKeyChecking no" $RDBHOST "cd ${SCRIPTDIR}; nice -n 19 ${RDBSCRIPTFULL}" > ${LOGDIR}/rdb.txt 2>&1 &
fi

sleep 2   # wait a sec till RDB comes up. TODO: implement more robust solution

RDBOUTPUTFILE=${RDBOUTPUTFILE:-/tmp/rdb.csv}
rm -f $RDBOUTPUTFILE
log "Starting RDB stat collector..."
nohup ${QHOME}/${PLATFORM}/q RDBStatCollector.q -rdb $RDBHOST:$RDBPORT -output $RDBOUTPUTFILE > ${LOGDIR}/rdbStatCollector.txt 2>&1 &

log "Starting publisher with frequency $FREQ for duration $DURATION"
nohup ${TIMERPRECOMMAND} ../bin/KDBPublishLatencyTester $FREQ $DURATION \
    $TIMEROUTPUTFILE $RDBHOST $RDBPORT $FLUSH $TIMERSTATONLY $BATCHSIZE $BATCHTYPE > ${LOGDIR}/publisher.txt 2>&1 &
PUB_PID=$!

afterStartWork

if [[ $ISLOCAL == true && $OS == Linux ]]; then
    nohup perf stat -x " " -p $RDB_PID -e task-clock --log-fd 1 > ${LOGDIR}/perf.txt 2>&1 &
    PERF_PID=$!
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

wait

METAFILE=/tmp/rdbperfmeta.csv

log "Creating csv for the meta data..."
echo "frequency,duration,batchsize,RDB CPU Usage" > $METAFILE
echo "$FREQ,$DURATION,$BATCHSIZE,$RDBCPUUSAGE" >> $METAFILE
