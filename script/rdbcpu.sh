source log.sh

function getRDBCPUUsage {
    local -i RDB_PID=$1
    local LOGDIR=$2

    rm -f ${LOGDIR}/perf.txt

    if [[ $(uname) == Linux ]]; then
        log "Collecting CPU stat of RDB process (PID: $RDB_PID)"
        perf stat -x "," -p $RDB_PID -e task-clock --log-fd 1 > ${LOGDIR}/perf.txt 2>&1 &
        PERF_PID=$!
        log "Waiting for RDB to finish"
        if wait $RDB_PID; then
            kill -s SIGINT $PERF_PID
        else
            log "RDB died abnormally."
            return 3
        fi
    else
        echo ",,,,,0,CPUs utilized" > ${LOGDIR}/perf.txt  # mocking a dummy perf.txt
    fi

    return 0
}