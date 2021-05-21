#!/usr/bin/env bash

set -eu

for folder in */; do
	folder="${folder%/}"
	echo "$*    $folder"
	cd "$folder" && "$@" ; cd ..
done
