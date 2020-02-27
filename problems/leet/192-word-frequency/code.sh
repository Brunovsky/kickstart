#!/bin/bash

# instead of using 'sed -r '/^\s*$/d' to delete empty lines, use tr -s
# tr can't read file, must be passed through stdin, so use cat
# in awk, use '$2, $1' instead of '$2 " " $1'

cat words.txt | \
	tr -s " " "\n" | \
	sort | \
	uniq -c | \
	sort -nr | \
	awk '{print $2, $1}'
