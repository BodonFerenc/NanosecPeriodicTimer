#!/usr/bin/env bash

set -eu -o pipefail

IFS=','     # to process csv files

GROUPEDOPT=''      # grouped attribute on sym
CONTINUE=0
NOCLEAN=0
FLUSHOPT=''
RDBHOST='0.0.0.0'
DURATION=60

declare -i STARTFREQ=50000
declare -i STARTBATCH=1

function args()
{
    options=$(getopt -o g --long cont --long noclean --long flush --long rdbhost: --long dur: --long startfreq: --long startbatchsize: -- "$@")
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
        --cont)
            CONTINUE=1
            ;;
        --noclean)
            NOCLEAN=1
            ;;
        --flush)
            FLUSHOPT='--flush'
            ;;
        --rdbhost)
            shift;
            RDBHOST=$1
            ;;
        --dur)
            shift;
            DURATION=$1
            ;;
        --startfreq)
            shift;
            STARTFREQ=$1
            ;;
        --startbatchsize)
            shift;
            STARTBATCH=$1
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

OUTPUTDIR=../res
mkdir -p $OUTPUTDIR
[[ $CONTINUE -ne 1 ]] && rm -f ${OUTPUTDIR}/statistics_*.csv   # previous run was cancelled


declare -i MINFREQ=1000
declare -i STEPFREQ=1000

declare -i MAXBATCHSIZE=100000

declare -i BATCHSIZE=STARTBATCH
declare -i FREQ=STARTFREQ

SINGLEMEASURESCRIPT=${SINGLEMEASURESCRIPT:-"./measureKdbLatency.sh"}

echo "Determining median of timer latency on current hardware"
echo "Running a timer with a simple task..."
../bin/PeriodicTimerDemo bytimerstrict 5000 5 /tmp/raw.csv
declare -i STARTMEDPUBLATLIMIT=$(q <<< 'exec `long$1+med latency from ("JJJ";enlist",") 0:hsym `$"/tmp/raw.csv"')
echo "Limit of the median timer latency is set to $STARTMEDPUBLATLIMIT "

while (( FREQ > MINFREQ  && BATCHSIZE < MAXBATCHSIZE )) ; do
	echo "running test with batchsize $BATCHSIZE ..."
	echo "running test with frequency $FREQ ..."

	statFilename=$OUTPUTDIR/statistics_${FREQ}_${BATCHSIZE}.csv
    STARTMEDPUBLATLIMIT=$STARTMEDPUBLATLIMIT $SINGLEMEASURESCRIPT $GROUPEDOPT $FLUSHOPT --rdbhost $RDBHOST --freq $FREQ --dur $DURATION --output ${statFilename} --batchsize $BATCHSIZE --batchtype batch

	stat=($(tail -n 1 $statFilename))
    declare -i ISSTABLEFLAG=${stat[-1]}

	if (( ISSTABLEFLAG )); then
        FREQRAW=$(echo "$FREQ * 0.95" | bc -l)
        FREQ=${FREQRAW%%.*}
		echo "frequency decreased"
	else
        BATCHSIZERAW=$(echo "1 + $BATCHSIZE * 1.05" | bc -l)
        BATCHSIZE=${BATCHSIZERAW%%.*}
	fi
done

echo "generating summary file $OUTPUTDIR/summary.csv"
cat $OUTPUTDIR/statistics_* | sort -n | uniq > $OUTPUTDIR/summaryTemp.csv
q postProcBatchSizeUpperBound.q $OUTPUTDIR/summaryTemp.csv $OUTPUTDIR/largestBatchSize.csv

paste -d, $OUTPUTDIR/summaryTemp.csv $OUTPUTDIR/largestBatchSize.csv > $OUTPUTDIR/summary.csv

if [[ $NOCLEAN -ne 1 ]]; then
    echo "Cleaning up temporal files..."
    rm ${OUTPUTDIR}/statistics_*.csv $OUTPUTDIR/summaryTemp.csv $OUTPUTDIR/largestBatchSize.csv
fi


exit 0