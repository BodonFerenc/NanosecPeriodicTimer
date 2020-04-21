#!/usr/bin/env bash

set -eu -o pipefail

source handleCommandlineArgs.sh

RDBPORT=5001
TIMERSTATONLY=1   # we dont need detailed timer output, just a summary
TIMEROUTPUTFILE=/tmp/timerstat.csv
RDBSCRIPT="rdb_latency.q"

function afterStartWork() {
    # do nothing
    :
}

source startPublisherAndRDB.sh

log "Merging meta data with timer and RDB statistics into $OUTPUT"
paste -d, $METAFILE $TIMEROUTPUTFILE $RDBOUTPUTFILE > $OUTPUT

if [[ $NOCLEAN -ne 1 ]]; then
    echo "Cleaning up temporal files..."
    rm $METAFILE $TIMEROUTPUTFILE $RDBOUTPUTFILE
fi


exit 0