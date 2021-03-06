#!/usr/bin/env bash

set -euo pipefail

GROUPEDOPT=''      # grouped attribute on sym
NOCLEAN=0
FLUSHOPT=''
OUTPUTDIR=../out
OUTPUT=${OUTPUTDIR}/summary.csv
RDBHOSTOPT=''
FREQS='10000 30000 60000 100000 150000'
DURS=60
BATCHSIZES=0
BATCHTYPE=cache


function args()
{
    options=$(getopt -o g --long noclean --long flush --long rdbhost: --long freq: --long dur: --long outputdir: --long output: --long batchsize: --long batchtype: -- "$@")
    [ $? -eq 0 ] || {
        echo "Incorrect option provided"
        exit 1
    }
    eval set -- "$options"
    while true; do
        case "$1" in
        -g)
            GROUPEDOPT='-g'
            ;;
        --noclean)
            NOCLEAN=1
            ;;
        --flush)
            FLUSHOPT='--flush'
            ;;
        --rdbhost)
            shift;
            RDBHOSTOPT="--rdbhost $1"
            ;;
        --freq)
            shift;
            FREQS=$(echo $1 | tr "," " ")
            ;;
        --dur)
            shift;
            DURS=$(echo $1 | tr "," " ")
            ;;
        --outputdir)
            shift;
            OUTPUTDIR=$1
            ;;
        --output)
            shift;
            OUTPUT=$1
            ;;
        --batchsize)
            shift;
            BATCHSIZES=$(echo $1 | tr "," " ")
           ;;
       --batchtype)
            shift;
            BATCHTYPE=$1
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

SINGLEMEASURESCRIPT=${SINGLEMEASURESCRIPT:-"./measureKdbLatency.sh"}

for FREQ in $(echo $FREQS); do
	echo "Running test with frequency $FREQ ..."
	for DUR in $(echo $DURS); do
		echo "Running test with duration $DUR ..."
        for BATCHSIZE in $(echo $BATCHSIZES); do
            echo "Running test with batch size $BATCHSIZE ..."
            PARTIALOUTPUT=${OUTPUTDIR}/statistics_${FREQ}_${DUR}_${BATCHSIZE}.csv
		    $SINGLEMEASURESCRIPT $GROUPEDOPT $FLUSHOPT $RDBHOSTOPT --freq $FREQ --dur $DUR --output ${PARTIALOUTPUT} --batchsize $BATCHSIZE --batchtype $BATCHTYPE
            tail -n 1 ${OUTPUT} >> ${OUTPUTDIR}/summary.csv
        done
	done
done

echo "Generating summary file ${OUTPUT}"
cat ${OUTPUTDIR}/statistics_*.csv | sort -n | uniq > ${OUTPUT}

if [[ $NOCLEAN -ne 1 ]]; then
    echo "Cleaning up temporal files..."
    rm ${OUTPUTDIR}/statistics_*.csv
fi

exit 0