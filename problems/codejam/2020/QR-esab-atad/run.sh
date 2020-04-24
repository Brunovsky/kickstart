#!/usr/bin/env bash

set -euf

declare -a FDS

coproc FDS { ./hack; }

./judge.py 0 <&${FDS[0]} >&${FDS[1]}
