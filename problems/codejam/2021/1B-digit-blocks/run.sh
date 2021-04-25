#!/bin/bash

if [[ "${1}" = --from-make ]]; then
    shift
else
    make
fi

SET="${1:-0}"
python3 interactive_runner.py python3 testing_tool.py "$SET" -- ./solver
