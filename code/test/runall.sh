#!/bin/bash

set -eu -o pipefail

for file in bin/*; do
	file=${file#bin/}
	echo ===== "$file"
	/usr/bin/time -f "# %es $file" "./bin/$file" || exit 1
	sleep 1s
done
