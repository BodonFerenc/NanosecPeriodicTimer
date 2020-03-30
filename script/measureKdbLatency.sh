#!/usr/bin/env bash

set -eu -o pipefail

RDBOUTPUTFILENAME=/tmp/rdb.csv
RDBSCRIPT="rdb_latency.q -output $RDBOUTPUTFILENAME"

source startPublisherAndRDB.sh

log "Merging meta data with publisher and RDB statistics into $OUTPUT"
paste -d, $METAFILE $tempOutputFileName $RDBOUTPUTFILENAME > $OUTPUT

rm $METAFILE

exit 0