#!/usr/bin/env bash

set -eu -o pipefail

VERBOSE=1
FREQ=30000
DURATION=10
PUBLISHEROUTPUT=/tmp/timer.csv
OUTPUT=stat.csv
KDBHOST=0.0.0.0
RDBOUTPUTFILENAME=/tmp/rdb.csv

function args()
{
    options=$(getopt -o q --long tcp --long freq: --long dur: --long output: -- "$@")
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
        --freq)
            shift; # The arg is next in position args
            FREQ=$1
            ;;
        --dur)
            shift; # The arg is next in position args
            DURATION=$1
            ;;
        --output)
            shift; # The arg is next in position args
            OUTPUT=$1
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

nohup perf stat -x " " -p $RDB_PID -e task-clock --log-fd 1 > ${LOGDIR}/perf.txt 2>&1 &
PERF_PID=$!

sleep 1

log

log "Starting publisher with frequency $FREQ for duration $DURATION"
numactl --physcpubind=1 ../bin/KDBPublishLatencyTester $FREQ $DURATION $PUBLISHEROUTPUT $KDBHOST 5001 > ${LOGDIR}/publisher.txt

tempOutputFileName=/tmp/stat.csv
log
log "Processing output $PUBLISHEROUTPUT"
q generatePublisherLatencyStats.q $PUBLISHEROUTPUT $tempOutputFileName -q

log "Waiting for perf stat (PID: $PERF_PID) and RDB (PID: $RDB_PID) to finish"
wait $PERF_PID

RDBCPUUSAGE=($(cat ${LOGDIR}/perf.txt))
log "RDB CPU usage ${RDBCPUUSAGE[5]}"

METAFILE=/tmp/rdbperfmeta.csv

log "Creating csv for the meta data..."
echo "frequency,duration,RDB CPU Usage" > $METAFILE
echo "$FREQ,$DURATION,${RDBCPUUSAGE[5]}" >> $METAFILE

log "Merging meta data with publisher and RDB statistics into $OUTPUT"
paste -d, $METAFILE $tempOutputFileName $RDBOUTPUTFILENAME > $OUTPUT

rm $METAFILE

exit 0