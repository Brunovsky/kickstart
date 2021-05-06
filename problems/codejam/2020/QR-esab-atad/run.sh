#!/usr/bin/env bash

set -euf

declare -a FDS

coproc FDS { ./solver; }

./judge.py 0 <&${FDS[0]} >&${FDS[1]}
