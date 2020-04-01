VERBOSE=1
FREQ=30000
DURATION=10
PUBLISHEROUTPUT=/tmp/timer.csv
OUTPUT=stat.csv
KDBHOST=0.0.0.0
FLUSH=0
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
    (( VERBOSE )) && echo "$@"
}

args $0 "$@"


LOGDIR=../log
mkdir -p $LOGDIR

log "Starting RDB script in the background..."
nohup numactl --physcpubind=0 q ${RDBSCRIPT} -p 5001 -q 2>&1 > ${LOGDIR}/rdb.txt 2>&1 &
RDB_PID=$!

sleep 1   # wait a sec till RDB comes up. TODO: implement more robust solution

log

log "Starting publisher with frequency $FREQ for duration $DURATION"
nohup numactl --physcpubind=1 ../bin/KDBPublishLatencyTester $FREQ $DURATION \
    $PUBLISHEROUTPUT $KDBHOST 5001 $FLUSH $BATCHSIZE $BATCHTYPE > ${LOGDIR}/publisher.txt 2>&1 &
PUB_PID=$!

afterStartWork

nohup perf stat -x " " -p $RDB_PID -e task-clock --log-fd 1 > ${LOGDIR}/perf.txt 2>&1 &
PERF_PID=$!

wait $PUB_PID
TIMERSTATFILE=/tmp/timerstat.csv
log
log "Processing output $PUBLISHEROUTPUT"
q generatePublisherLatencyStats.q $PUBLISHEROUTPUT $TIMERSTATFILE -q

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
