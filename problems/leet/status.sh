#!/bin/bash

# Usage: ./status.sh

ls -1Fv | grep -Ee '^[0-9]+-.+/' | while read -r folder; do
    if ! test -f "$folder/README.md"; then
        continue
    fi
    echo -e "  " $(sed '5q;d' $folder/README.md) "\t" "$folder"
done
