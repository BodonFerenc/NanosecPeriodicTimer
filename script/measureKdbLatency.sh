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

TEMPOUTPUT=/tmp/summary.csv
log "Merging meta data with timer and RDB statistics into $TEMPOUTPUT"
paste -d, $METAFILE $TIMEROUTPUTFILE $RDBOUTPUTFILE > $TEMPOUTPUT

ISSTABLEFILE=/tmp/isstable.csv
echo "isStable" > $ISSTABLEFILE
getISStableFlag $TEMPOUTPUT
log "Is stable flag: $ISSTABLEFLAG"
echo $ISSTABLEFLAG >> $ISSTABLEFILE

log "Merging temp result file and is stable file to $OUTPUT"
paste -d, $TEMPOUTPUT $ISSTABLEFILE > $OUTPUT


if [[ $NOCLEAN -ne 1 ]]; then
    echo "Cleaning up temporal files..."
    rm $METAFILE $TIMEROUTPUTFILE $RDBOUTPUTFILE $ISSTABLEFILE $TEMPOUTPUT
fi

exit 0