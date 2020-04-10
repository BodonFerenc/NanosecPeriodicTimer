#!/usr/bin/env bash

set -eu -o pipefail

source handleCommandlineArgs.sh

RDBPORT=5001
RDBOUTPUTFILE=${RDBOUTPUTFILE:-/tmp/rdb.csv}
rm -f $RDBOUTPUTFILE
RDBSCRIPT="rdb_latency.q -output $RDBOUTPUTFILE"

function afterStartWork() {
    # do nothing
    :
}

source startPublisherAndRDB.sh

# This is for slow NFS filesystems
log "Waiting for RDB output file to be available"
i=0
sp="/-\|"
while [[ ! -f $RDBOUTPUTFILE ]]; do
    printf "\b${sp:i++%${#sp}:1}"
    sleep 1
done
log "Waited $i seconds for RDB output to be available"

log "Merging meta data with timer and RDB statistics into $OUTPUT"
paste -d, $METAFILE $TIMERSTATFILE $RDBOUTPUTFILE > $OUTPUT

rm $METAFILE

exit 0