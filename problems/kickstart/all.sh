#!/bin/bash

# Usage: ./all.sh CMD [makeargs...]

CMD="$1"
shift

for folder in 20*/*/; do
	folder="${folder%/}"
	echo "$CMD  $folder"
	make -s -C "$folder" "$@" | sed 's/^/  /'
done
