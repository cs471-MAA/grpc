#!/usr/bin/env bash
# Make script exit and not continue if a command fail
set -eu
set -o pipefail
# set -xv # for debug


tc qdisc add dev eth0 root netem delay "$1" "$2" "$3%"

eval "$4"