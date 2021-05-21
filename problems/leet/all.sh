#!/bin/bash

set -eu

ls -1Fv | grep -Ee '^[0-9]+-.+/' | while read -r folder; do
    folder="${folder%/}"
    if test -f "$folder/README.md" && test ! -f "$folder/.skip"; then
        echo "$*    $folder"
        cd "$folder" ; $* ; cd ..
    fi
done
