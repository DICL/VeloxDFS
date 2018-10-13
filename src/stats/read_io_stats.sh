#!/usr/bin/env bash

# Those variables are configurable
# -------------------------------------------------------------------
VELOX_GET_OPS="velox_get_config"

VARERR=""

# Error reporting code
# -------------------------------------------------------------------
err_report() { echo "Critical error at line: $(caller)"; }
trap err_report ERR 

die() { echo "$* $VARERR" && exit 2; }

# call ./read_io_stats.sh debug to debug this script
[ "$1" = "debug" ] && set -x   # enable debbuging output
set -e                         # Exit on failed command

# Utility functions
# -------------------------------------------------------------------
function get_opt() {
    local getops_binary=$1
    local output=$($getops_binary $2)
    VARERR=$output

    case "$?" in
        0) echo $output;;
    127) echo "I can't find velox_get_config program"; exit 3;;
        *) echo "Option $2 could not be found in config file"; exit 4;;
    esac
}

function zk_query() {
    local QUERY=$1
    local ZK_SERVER_ADDR=`get_opt $VELOX_GET_OPS "addons.zk.addr"`
    local ZK_SERVER_PORT=`get_opt $VELOX_GET_OPS "addons.zk.port"`
    local ZKCLIENT=`get_opt $VELOX_GET_OPS "addons.zk.client_path"`

    # Launch query
    local output=$($ZKCLIENT -server $ZK_SERVER_ADDR:$ZK_SERVER_PORT $QUERY 2>&1)
    local exit_code=$?
    VARERR=$output

    ( [[ $output =~ ERROR|Exception ]] || [ $exit_code -ne 0 ] ) && echo "$output" && return 2

    echo "$output"
}

# Main logic
# -------------------------------------------------------------------
SERVER_ID=`get_opt $VELOX_GET_OPS "id"`
DEVICE=`get_opt $VELOX_GET_OPS "addons.disk_id"`
TICK=`get_opt $VELOX_GET_OPS "addons.disk_monitor_tick"`
NUM_CORES=`grep -Pom 1 'cpu cores\s*:\s\K\d' /proc/cpuinfo`

# Create parents ZNODES
zk_query "create /stats dummy" >/dev/null || die "Error in creating parent folders"
zk_query "create /stats/io dummy" >/dev/null || die "Error in creating parent folders"

# Forever loop to keep reporting
(iostat $DEVICE -x -d $TICK | awk '/'"^$DEVICE"'/ { print $14; fflush() }') | while read line; do
    #if [ $line != "0.00" ]; then
        output=`zk_query "get /stats/io/$SERVER_ID"`
        load=`uptime | grep -Po 'load average: \K\d+.\d\d' | awk '{print int($1 + 0.5)}'`
        free_cpus=$(($NUM_CORES * 2  - $load))
        if (($free_cpus < 0)); then
            free_cpus=0
        fi

        set +e  # Needed for grep exit code

        # Check for created ZNODE
        grep -qE '^[[:digit:]]{1,20}\.[[:digit:]]{1,2},\-{0,1}[[:digit:]]{1,2}$' <<< "$output"
        if [ $? -eq 0 ]; then
            QUERY="set /stats/io/$SERVER_ID ${line},$free_cpus"
        else  
            QUERY="create /stats/io/$SERVER_ID ${line},$free_cpus"
        fi

        set -e
        zk_query "$QUERY" >/dev/null || die "Error setting the io value in ZK"

    #fi 
done
# vim: ts=4 : sw=4
