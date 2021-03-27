#!/bin/bash

if test "${1}" = --from-make; then
    shift
else
    make
fi

python3 testing_tool.py ./hack
