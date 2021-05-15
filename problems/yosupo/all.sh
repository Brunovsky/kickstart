#!/usr/bin/env bash

set -eu

ls -1Fv | grep -Eve 'template.*/' | grep -Ee '.+/' | while read -r folder; do
	folder="${folder%/}"
	echo "$*    $folder"
	cd "$folder" && "$@" ; cd ..
done
