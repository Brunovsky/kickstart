#!/bin/bash

nlines=$(head -n1 file.txt | tr -s ' ' '\n' | wc -l)
for i in $(seq 1 $nlines); do
    cut -f$i -d' ' file.txt | tr -s '\n' ' '
    printf '\n'
done | awk '{$1=$1};1'

# or one-liner:
#seq 1 $nlines | xargs -I '{}' sh -c "cut -f'{}' -d' ' file.txt | tr -s '\n' ' ' ; printf '\n'" \
#	| awk '{$1=$1};1'
