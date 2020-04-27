#!/usr/bin/env bash

VERBOSE=1
GROUPEDOPT=''      # grouped attribute on sym
NOCLEAN=0
FLUSH=
RDBHOST=0.0.0.0    # default: using unix socket
declare -i FREQ=30000
declare -i DURATION=10
OUTPUT=stat.csv
declare -i BATCHSIZE=0
BATCHTYPE='nobatch'

OS=$(uname)
case $OS in
    Linux)
        PLATFORM=l64
        RDBPRECOMMAND="numactl --physcpubind=0"
        TIMERPRECOMMAND="numactl --physcpubind=1"
        ;;
    Darwin)
        PLATFORM=m64
        RDBPRECOMMAND=
        TIMERPRECOMMAND=
        ;;
    *)
        echo "Not supported operating system $OS."
        exit 2
esac

function args()
{
    options=$(getopt -o q -o g --long noclean --long flush --long rdbhost: --long freq: --long dur: --long output: --long batchsize: --long batchtype: -- "$@")
    [ $? -eq 0 ] || {
        echo "Incorrect option provided"
        exit 1
    }
    eval set -- "$options"
    while true; do
        case "$1" in
        -q)
            VERBOSE=0
            ;;
        -g)
            GROUPEDOPT='-grouped'
            ;;
        --noclean)
            NOCLEAN=1
            ;;
        --flush)
            FLUSH='-f'
            ;;
        --rdbhost)
            shift;
            RDBHOST=$1
            ;;
        --freq)
            shift;
            FREQ=$1
            ;;
        --dur)
            shift;
            DURATION=$1
            ;;
        --output)
            shift;
            OUTPUT=$1
           ;;
        --batchsize)
            shift;
            BATCHSIZE=$1
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