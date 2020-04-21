#!/usr/bin/env bash

set -eu -o pipefail

source handleCommandlineArgs.sh

TIMERSTATONLY=1   # we dont need detailed timer output, just a summary
TIMEROUTPUTFILE=/tmp/timerstat.csv
RDBSCRIPT="rdb_latency.q"

function afterStartWork() {
    # do nothing
    :
}

source startPublisherAndRDB.sh

exit 0