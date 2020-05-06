#!/usr/bin/env bash
nice -n 19 ${RDBSCRIPTFULL} &
RDB_PID=$!

source rdbcpu.sh
getRDBCPUUsage ${RDB_PID} ${LOGDIR}

exit 0