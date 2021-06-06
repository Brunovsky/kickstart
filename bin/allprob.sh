#!/usr/bin/env bash

declare -a FILES=('code.cpp' 'Makefile')

for dir in $(find . -mindepth 1 -maxdepth 4 -type d | sort); do
	ok=1
	for file in "${FILES[@]}"; do
		if test ! -f "$dir/$file"; then
			ok=0
			break
		fi
	done
	if test "$ok" = 1; then
		echo "=== $dir"
		(cd "$dir" && "$@")
	fi
done
