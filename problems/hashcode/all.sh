#!/usr/bin/env bash

set -eu

for folder in 20*/; do
	folder="${folder%/}"
	echo "$*    $folder"
	cd "$folder" && "$@" ; cd ..
done
