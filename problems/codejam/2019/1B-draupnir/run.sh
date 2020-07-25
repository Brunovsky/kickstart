#!/bin/bash

SET="${1:-0}"

make
python3 interactive_runner.py python3 testing_tool.py "$SET" -- ./hack
