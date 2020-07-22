#!/bin/bash

# Usage: ./all.sh CMD [makeargs...]

CMD="$1"
shift

ls -1Fv | grep -Ee '^[0-9]+-.+/' | while read -r folder; do
    folder="${folder%/}"
    if test -f "$folder/README.md" && ! test -f "$folder/.skip"; then
        echo "$CMD  $folder"
        make -s -C "$folder" "$@" | sed 's/^/  /'
    fi
done
