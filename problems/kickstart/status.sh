#!/bin/bash

# Usage: ./status.sh

for year in 20*; do
    echo "$year"
    for folder in $year/*/; do
        if test -f "$folder/.skip"; then
            continue
        fi

        echo -e "  " $(sed '5q;d' $folder/README.md) "\t" "$folder"
    done
done
