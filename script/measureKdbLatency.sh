#!/usr/bin/env bash

set -eu -o pipefail

source handleCommandlineArgs.sh

RDBPORT=5001
RDBOUTPUTFILE=/tmp/rdb.csv
RDBSCRIPT="numactl --physcpubind=0 q rdb_latency.q $GROUPEDOPT -output $RDBOUTPUTFILE -p $RDBPORT -q"

function afterStartWork() {
    # do nothing
    :
}

source startPublisherAndRDB.sh

log "Merging meta data with timer and RDB statistics into $OUTPUT"
paste -d, $METAFILE $TIMERSTATFILE $RDBOUTPUTFILE > $OUTPUT

rm $METAFILE

exit 0