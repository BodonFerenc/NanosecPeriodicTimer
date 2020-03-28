#!/usr/bin/env bash

set -eu -o pipefail

VERBOSE=1
FREQ=30000
DURATION=10
PUBLISHEROUTPUT=/tmp/timer.csv
OUTPUT=stat.csv
KDBHOST=0.0.0.0
FLUSH=0
RDBOUTPUTFILENAME=/tmp/rdb.csv
BATCHSIZE=0
BATCHTYPE=cache

function args()
{
    options=$(getopt -o q --long tcp --long flush --long freq: --long dur: --long output: --long batchsize: --long batchtype: -- "$@")
    [ $? -eq 0 ] || {
        echo "Incorrect option provided"
        exit 1
    }
    eval set -- "$options"
    while true; do
        case "$1" in
        -q)
            VERBOSE=0
            ;;
        --tcp)
            KDBHOST=localhost
            ;;            
        --flush)
            FLUSH=1
            ;;
        --freq)
            shift;
            FREQ=$1
            ;;
        --dur)
            shift;
            DURATION=$1
            ;;
        --output)
            shift;
            OUTPUT=$1
           ;;
        --batchsize)
            shift;
            BATCHSIZE=$1
           ;;           
       --batchtype)
            shift;
            BATCHTYPE=$1
           ;;               
        --)
            shift
            break
            ;;
        esac
        shift
    done
}

function log () {
    if [[ $VERBOSE -eq 1 ]]; then
        echo "$@"
    fi
}

args $0 "$@"


LOGDIR=../log
mkdir -p $LOGDIR


log "Starting RDB script in the background..."
nohup numactl --physcpubind=0 q rdb_latency.q -output $RDBOUTPUTFILENAME -p 5001 2>&1 > ${LOGDIR}/rdb.txt 2>&1 &
RDB_PID=$!

sleep 1   # wait a sec till RDB comes up. TODO: implement more robust solution

log

log "Starting publisher with frequency $FREQ for duration $DURATION"
numactl --physcpubind=1 ../bin/KDBPublishLatencyTester $FREQ $DURATION $PUBLISHEROUTPUT $KDBHOST 5001 $FLUSH $BATCHSIZE $BATCHTYPE> ${LOGDIR}/publisher.txt

nohup perf stat -x " " -p $RDB_PID -e task-clock --log-fd 1 > ${LOGDIR}/perf.txt 2>&1 &
PERF_PID=$!

tempOutputFileName=/tmp/stat.csv
log
log "Processing output $PUBLISHEROUTPUT"
q generatePublisherLatencyStats.q $PUBLISHEROUTPUT $tempOutputFileName -q

wait $RDB_PID

log "Waiting for perf stat (PID: $PERF_PID) and to finish"
if wait $PERF_PID; then
    RDBCPUUSAGESTAT=($(cat ${LOGDIR}/perf.txt))
    RDBCPUUSAGE=${RDBCPUUSAGESTAT[5]}
    log "RDB CPU usage $RDBCPUUSAGE"
else
    # TODO: invetigate perf stat errors
    log "Error measuring RDB CPU usage, command perf failed."
    RDBCPUUSAGE=''
fi

METAFILE=/tmp/rdbperfmeta.csv

log "Creating csv for the meta data..."
echo "frequency,duration,batchsize,RDB CPU Usage" > $METAFILE
echo "$FREQ,$DURATION,$BATCHSIZE,$RDBCPUUSAGE" >> $METAFILE

log "Merging meta data with publisher and RDB statistics into $OUTPUT"
paste -d, $METAFILE $tempOutputFileName $RDBOUTPUTFILENAME > $OUTPUT

rm $METAFILE

exit 0