#!/bin/bash

if test "${1}" = --from-make; then
    shift
else
    make
fi

python3 interactive_runner.py ./judge "$@" -- ./solver
