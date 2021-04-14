#!/bin/bash

set -eu -o pipefail
mkdir -p ./output

for file in bin/*; do
	file=${file#bin/}
	echo ===== "$file"
	/usr/bin/time -f "# %es $file" "./bin/$file" | tee "./output/$file"
	sleep 1s
done
