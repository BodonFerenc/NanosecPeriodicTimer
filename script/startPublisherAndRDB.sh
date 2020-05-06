#!/usr/bin/env bash

# Abnormal exit codes:
# 2: invalid RDB host
# 3: RDB died abnormally

LOGDIR=../log
mkdir -p $LOGDIR

RDBPORT=5001
RDBSCRIPTFULL="${RDBPRECOMMAND} ${QHOME}/${PLATFORM}/q ${RDBSCRIPT} $GROUPEDOPT -q -exit -p $RDBPORT"
if [[ $RDBHOST == 0.0.0.0 || $RDBHOST == localhost ]]; then
    ISLOCAL=true
    log "Starting RDB script in the background..."
    ${RDBSCRIPTFULL} > ${LOGDIR}/rdb.txt 2>&1 &
    RDB_PID=$!
else
    if [[ -z $(dig +short $RDBHOST) ]]; then
        log "Invalid RDB address!"
        exit 2
    fi
    ISLOCAL=false
    SCRIPTDIR=$(pwd)
    ssh -o "StrictHostKeyChecking no" $RDBHOST "cd ${SCRIPTDIR}; RDBSCRIPTFULL=\"${RDBSCRIPTFULL}\" LOGDIR=${LOGDIR} ./startRemoteRDB.sh" > ${LOGDIR}/rdb.txt 2>&1 &
fi

# Waiting for the RDB to be responsive
while ! nc -z $RDBHOST $RDBPORT; do sleep 0.1; done

RDBOUTPUTFILE=${RDBOUTPUTFILE:-/tmp/rdb.csv}
if [[ $RDBHOST == 0.0.0.0 ]]; then
    RDBQADDRESS="unix://$RDBPORT"
else
    RDBQADDRESS="$RDBHOST:$RDBPORT"
fi
rm -f $RDBOUTPUTFILE
log "Starting RDB stat collector..."
${QHOME}/${PLATFORM}/q RDBStatCollector.q -rdb $RDBQADDRESS -output $RDBOUTPUTFILE > ${LOGDIR}/rdbStatCollector.txt 2>&1 &

rm -f $TIMEROUTPUTFILE
log "Starting publisher with frequency $FREQ for duration $DURATION"
${TIMERPRECOMMAND} ../bin/KDBPublishLatencyTester $FREQ $DURATION \
    $TIMEROUTPUTFILE $RDBHOST $RDBPORT $FLUSH $TIMERSTATONLY -b $BATCHSIZE -t $BATCHTYPE > ${LOGDIR}/publisher.txt 2>&1 &
PUB_PID=$!

afterStartWork

source rdbcpu.sh

if [[ $ISLOCAL == true ]]; then
    getRDBCPUUsage $RDB_PID $LOGDIR
else
    log "Waiting for RDB to become unresponsive"
    while nc -z $RDBHOST $RDBPORT > /dev/null; do sleep 1; done
    scp $RDBHOST:${SCRIPTDIR}/${LOGDIR}/perf.txt $LOGDIR
fi

IFS=,
RDBCPUUSAGESTAT=($(cat ${LOGDIR}/perf.txt))
RDBCPUUSAGE=${RDBCPUUSAGESTAT[5]}
log "RDB CPU usage $RDBCPUUSAGE"

wait

log "Creating csv for the meta data..."
METAFILE=/tmp/rdbperfmeta.csv
echo "frequency,duration,batchsize,RDB CPU Usage" > $METAFILE
echo "$FREQ,$DURATION,$BATCHSIZE,$RDBCPUUSAGE" >> $METAFILE

function getISStableFlag {
    local statFilename=$1

    log "Determining if ingest was stable"
    ISSTABLEFLAG=0

    local IFS=,
    local -a stat=($(tail -n 1 $statFilename))

    local RDBTIMERAW=${stat[9]}
    local -i RDBTIME=${RDBTIMERAW%%.*}
    log "RDB ingest time was $RDBTIME"
    if (( RDBTIME > DURATION + 1 )); then return 0; fi

    MEDLATLIMITOFFSET=${MEDLATLIMITOFFSET:-1000000}
    local -i MEDLATLIMIT
    if [[ $BATCHSIZE -gt 0 && $BATCHTYPE == "cache" ]]; then
        MEDLATLIMIT=$(( MEDLATLIMITOFFSET + 1000000000 * BATCHSIZE / FREQ ))
    else
        MEDLATLIMIT=$MEDLATLIMITOFFSET     # limit for the median of the latency
    fi

    local -i MEDLAT=${stat[16]}
    log "Median latency was $MEDLAT, the limit is $MEDLATLIMIT"
    if (( MEDLAT > MEDLATLIMIT )); then return 0; fi


	local -i MEDPUBLAT=${stat[8]}
    log "Median of publication latency was $MEDPUBLAT"

    if [[ -z ${STARTMEDPUBLATLIMIT+dontcare} ]]; then
        log "Determining median of timer latency on current hardware"
        log "Running a timer with a simple task..."
        ../bin/PeriodicTimerDemo bytimerstrict 5000 5 /tmp/raw.csv
        declare -i STARTMEDPUBLATLIMIT=$(q <<< 'exec `long$1+med latency from ("JJJ";enlist",") 0:hsym `$"/tmp/raw.csv"')
    fi
    local -i MEDPUBLATLIMIT
    if [[ $BATCHSIZE -gt 0 && $BATCHTYPE == "cache" ]]; then
        MEDPUBLIMITOFFSET=${MEDPUBLIMITOFFSET:-200000}
        MEDPUBLATLIMIT=$(( STARTMEDPUBLATLIMIT + FREQ / MEDPUBLIMITOFFSET ))
        log "Limit for the median of publication latency is $MEDPUBLATLIMIT"
    else
        MEDPUBLATLIMIT=STARTMEDPUBLATLIMIT
    fi

    if (( MEDPUBLAT > MEDPUBLATLIMIT)); then return 0; fi

    ISSTABLEFLAG=1
    return 0
}

