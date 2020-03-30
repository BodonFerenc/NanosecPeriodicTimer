#!/usr/bin/env bash

set -eu -o pipefail

RDBOUTPUTFILE=/tmp/rdb.csv
RDBSCRIPT="rdb_latency.q -output $RDBOUTPUTFILE"

function afterStartWork {
    # do nothing
}

source startPublisherAndRDB.sh

log "Merging meta data with timer and RDB statistics into $OUTPUT"
paste -d, $METAFILE $TIMERSTATFILE $RDBOUTPUTFILE > $OUTPUT

rm $METAFILE

exit 0