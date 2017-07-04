#!/bin/bash
# Simple integration tests to test dfs put | get
# 1. Get generate a file ~20MiB
# 2. we upload it .
# 3. we downloaded and compare to the original file.
#
# We just use velox network with a single node for 
# constrictions of the Travis CI plataform.

trap cleanup SIGHUP SIGINT SIGTERM EXIT  # Always call cleanup

: ${FILE_SIZE:=20M}
 
node_pid=0
root_dir=`pwd`

die() { echo "$@" 1>&2 ; exit 1; }

function setup() {
  #create scratch folder
  [ -d scratch ] || mkdir scratch
  [ -d tmp ] || mkdir tmp

  # Setup mandatory variables
  export NETWORK_NODES=127.0.0.1
  export PATH_SCRATCH=`readlink -f scratch` 
  export PATH_IDATA=$PATH_SCRATCH
  export PATH_METADATA=$PATH_SCRATCH
  export PATH_APPLICATIONS=$PATH_SCRATCH
  export NETWORK_PORTS_CLIENT=4333
  export NETWORK_IFACE=lo

  #Generate sample file
  dd if=/dev/urandom bs=$FILE_SIZE count=1 | tr -dc 'a-z0-9' | fold -w 255 > sample.txt

  #Make sure I am the only instance
  pgrep eclipse_node &> /dev/null && pkill eclipse_node
}

function cleanup() {
  cd $root_dir
  rm sample.txt
  rm -rf tmp
  rm -rf scratch

  kill $node_pid
}


setup

./eclipse_node &
node_pid=$!

# Let it setup
sleep 1

./veloxdfs put sample.txt || die "Put file problem"

file scratch/sample.txt_0 &>/dev/null || die "DFS did not recieved the file"

cd tmp
../veloxdfs get sample.txt || die "Get file problem"

diff -s sample.txt ../sample.txt || die "Files are not identical"
