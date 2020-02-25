#!/bin/bash

# Usage: ./all.sh CMD [makeargs...]

CMD="$1"
shift

for makefile in **/*/Makefile; do
	folder=$(dirname "$makefile")
	echo "$CMD  $folder"
	make -s -C "$folder" "$@" | sed 's/^/  /'
done
