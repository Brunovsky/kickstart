#!/bin/bash

# Usage: ./all.sh CMD [makeargs...]

CMD="$1"
shift

for folder in */*/; do
	folder="${folder%/}"
	if test -f "$folder/.skip"; then
		continue
	fi
	echo "$CMD  $folder"
	make -s -C "$folder" "$@" | sed 's/^/  /'
done
