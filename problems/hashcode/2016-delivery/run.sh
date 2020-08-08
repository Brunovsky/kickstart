#!/usr/bin/env bash

set -euf

make
clear

RUN=""
if [ "$1" = "--valgrind" ]; then
    RUN="valgrind --max-stackframe=40000000"
    shift
fi

echo ===== hack "$@" =====
$RUN ./hack "$@"
