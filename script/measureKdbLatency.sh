#!/usr/bin/env bash

set -eu -o pipefail

source handleCommandlineArgs.sh

TIMERSTATONLY='-s'   # we dont need detailed timer output, just a summary
TIMEROUTPUTFILE=/tmp/timerstat.csv
RDBSCRIPT="rdb_latency.q"

function afterStartWork() {
    # do nothing
    :
}

source startPublisherAndRDB.sh

# make sure required data is written out
sync -d $METAFILE $TIMEROUTPUTFILE $RDBOUTPUTFILE

TEMPOUTPUT=/tmp/summary.csv
log "Merging meta data with timer and RDB statistics into $TEMPOUTPUT"
paste -d, $METAFILE $TIMEROUTPUTFILE $RDBOUTPUTFILE > $TEMPOUTPUT

ISSTABLEFILE=/tmp/isstable.csv
log "isStable" > $ISSTABLEFILE
getISStableFlag $TEMPOUTPUT
log "Is stable flag: $ISSTABLEFLAG"
log $ISSTABLEFLAG >> $ISSTABLEFILE

log "Merging temp result file and is stable file to $OUTPUT"
paste -d, $TEMPOUTPUT $ISSTABLEFILE > $OUTPUT


if [[ $NOCLEAN -ne 1 ]]; then
    log "Cleaning up temporal files..."
    rm $METAFILE $TIMEROUTPUTFILE $RDBOUTPUTFILE $ISSTABLEFILE $TEMPOUTPUT
fi

exit 0