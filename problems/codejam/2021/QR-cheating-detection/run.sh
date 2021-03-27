#!/bin/bash

make perfm
make fast | tee program.txt
diff -a --suppress-common-lines -y program.txt correct.txt
