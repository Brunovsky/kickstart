#!/usr/bin/env bash

set -eu

ls -1Fv | grep -Ee '^[0-9]+-.+/' | while read -r folder; do
	folder="${folder%/}"
	echo "$*    $folder"
	cd "$folder" && "$@" ; cd ..
done
