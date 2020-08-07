#!/bin/bash

# Usage: ./status.sh

for folder in 20*/; do
    folder="${folder%/}"
    if test -f "$folder/README.md"; then
        echo -e "  " $(sed '5q;d' "$folder/README.md") "\t" "$folder"
    fi
done
