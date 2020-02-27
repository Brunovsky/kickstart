#!/bin/bash

grep --color=never -Exe '(\([0-9]{3}\) |[0-9]{3}-)[0-9]{3}-[0-9]{4}' file.txt
