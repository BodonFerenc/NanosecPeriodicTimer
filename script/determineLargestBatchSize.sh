#!/usr/bin/env bash

set -eu -o pipefail

IFS=','     # to process csv files

NOCLEAN=0
FLUSH=''
TCP=''
DURATION=60

declare -i STARTFREQ=50000
declare -i STARTBATCH=1

function args()
{
    options=$(getopt --long noclean --long tcp --long flush --long dur: --long startfreq: --long startbatchsize: -- "$@")
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
        --tcp)
            TCP='--tcp'
            ;;   
        --flush)
            FLUSH='--flush'
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
rm -f ${OUTPUTDIR}/statistics_*.csv


declare -i ENDFREQ=6000000
declare -i STEPFREQ=10000
declare -i INCFREQ=500000   # increase increment

declare -i MAXBATCHSIZE=8000

declare -i MEDPUBLIMITOFFSET=300000

declare -i BATCHSIZE=STARTBATCH
declare -i FREQ=STARTFREQ

echo "Determining median of timer latency on current hardware"
echo "Running a timer with a simple task..."
../bin/PeriodicTimerDemo bytimerstrict 5000 5 /tmp/raw.csv
declare -i STARTMEDPUBLATLIMIT=$(q <<< 'exec `long$1+med latency from ("JJJ";enlist",") 0:hsym `$"/tmp/raw.csv"')

(( MEDPUBLATLIMIT=STARTMEDPUBLATLIMIT + FREQ / MEDPUBLIMITOFFSET ))


while (( FREQ < ENDFREQ  && BATCHSIZE < MAXBATCHSIZE )) ; do
	echo "running test with batchsize $BATCHSIZE ..."
	echo "running test with frequency $FREQ ..."
	
	statFilename=$OUTPUTDIR/statistics_${FREQ}_${BATCHSIZE}.csv 
    ./measureKdbLatency.sh $FLUSH $TCP --freq $FREQ --dur $DURATION --output ${statFilename} --batchsize $BATCHSIZE --batchtype cache

	stat=($(tail -n 1 $statFilename))
	declare -i RDBCPUUSAGE=$(echo "100 * ${stat[3]:-0} / 1" | bc)
	declare -i MEDPUBLAT=${stat[7]}
	echo "Median of publication latency was $MEDPUBLAT"
    echo "Current limit for the median of publication latency is $MEDPUBLATLIMIT"

	if (( MEDPUBLAT > MEDPUBLATLIMIT  || RDBCPUUSAGE > 95 )); then
        declare -i BATCHINC
        if (( BATCHSIZE > 20 )); then
            BATCHINCFLOAT=$(echo "l($BATCHSIZE)-1" | bc -l)
            BATCHINC=${BATCHINCFLOAT%%.*}
        else
            BATCHINC=1
        fi
		(( BATCHSIZE += BATCHINC ))
		(( MEDPUBLATLIMIT = STARTMEDPUBLATLIMIT + FREQ / MEDPUBLIMITOFFSET ))
		echo "batch size increased by $BATCHINC"
	else
        (( FREQ += STEPFREQ * (1 + FREQ / INCFREQ) ))
		(( FREQ += STEPFREQ ))
		echo "frequency increased"
	fi
done

echo "generating summary file $OUTPUTDIR/summary.csv"
cat $OUTPUTDIR/statistics_* | sort -n | uniq > $OUTPUTDIR/summary.csv
q postProcLargestBatchSizeCSV.q $OUTPUTDIR/summary.csv $OUTPUTDIR/summary.csv

if [[ $NOCLEAN -ne 1 ]]; then
    echo "Cleaning up temporal files..."
    rm ${OUTPUTDIR}/statistics_*.csv
fi


exit 0