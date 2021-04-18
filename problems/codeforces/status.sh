#!/bin/bash

# Usage: ./status.sh

ls -1Fv | grep -Ee '^[0-9]+-.+/' | while read -r folder; do
    folder="${folder%/}"
    if test -f "$folder/README.md"; then
        echo -e "  " $(sed '3q;d' $folder/README.md) "\t" "$folder"
    fi
done
