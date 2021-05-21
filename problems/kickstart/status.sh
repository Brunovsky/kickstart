#!/bin/bash

# Usage: ./status.sh

for folder in 20*; do
    if test -f "$folder/README.md"; then
        echo -e "  " $(sed '5q;d' "$folder/README.md") "\t" "$folder"
    else
        echo -e "  " "Unknown" "\t" "$folder"
    fi
done
