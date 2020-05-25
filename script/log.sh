function log {
    (( VERBOSE )) && echo $(date -u +"%d %b %X") "$@"
}