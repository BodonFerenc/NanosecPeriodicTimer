#!/usr/bin/env bash

VERBOSE=1
GROUPEDOPT=''      # grouped attribute on sym
FLUSH=0
RDBHOST=0.0.0.0    # default: using unix socket
FREQ=30000
DURATION=10
OUTPUT=stat.csv
BATCHSIZE=0
BATCHTYPE=cache


function args()
{
    options=$(getopt -o q -o g --long flush --long rdbhost: --long freq: --long dur: --long output: --long batchsize: --long batchtype: -- "$@")
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
        --flush)
            FLUSH=1
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