#!/usr/bin/env bash

set -eu -o pipefail

NOCLEAN=0
OUTPUTDIR=../out
FREQS='10000 30000 60000 100000 150000'
DURS='30 60'

function args()
{
    options=$(getopt --long noclean --long freq: --long dur: --long outputdir: -- "$@")
    [ $? -eq 0 ] || {
        echo "Incorrect option provided"
        exit 1
    }
    eval set -- "$options"
    while true; do
        case "$1" in
        --noclean)
            NOCLEAN=1
            ;;            
        --freq)
            shift; # The arg is next in position args
            FREQS=$(echo $1 | tr "," " ")
            ;;
        --dur)
            shift; # The arg is next in position args
            DURS=$(echo $1 | tr "," " ")
            ;;
        --outputdir)
            shift; # The arg is next in position args
            OUTPUTDIR=$1
            ;;
        --)
            shift
            break
            ;;
        esac
        shift
    done
}

args $0 "$@"

mkdir -p ${OUTPUTDIR}
rm -f ${OUTPUTDIR}/statistics_*.csv

for FREQ in $(echo $FREQS); do
	echo "Running test with frequency $FREQ ..."
	for DUR in $(echo $DURS); do
		echo "Running test with duration $DUR ..."
		./measureKdbLatency.sh --freq $FREQ --dur $DUR --output ${OUTPUTDIR}/statistics_${FREQ}_${DUR}.csv
        tail -n 1 ${OUTPUTDIR}/statistics_${FREQ}_${DUR}.csv >> ${OUTPUTDIR}/summary.csv
	done
done

echo "generating summary file ${OUTPUTDIR}/summary.csv"
cat ${OUTPUTDIR}/statistics_*.csv | sort -n | uniq > ${OUTPUTDIR}/summary.csv

if [[ $NOCLEAN -ne 1 ]]; then
    echo "cleaning up temporal files..."
    rm ${OUTPUTDIR}/statistics_*.csv
fi

exit 0