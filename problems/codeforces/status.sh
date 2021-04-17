#!/bin/bash

# Usage: ./status.sh

for year in 20*; do
    echo "$year"
    for folder in "$year"/*/; do
        folder="${folder%/}"
        if test -f "$folder/README.md"; then
            problem="${folder#"$year"/}"
            echo -e "  " $(sed '3q;d' "$folder/README.md") "\t" "$problem"
        fi
    done
done
