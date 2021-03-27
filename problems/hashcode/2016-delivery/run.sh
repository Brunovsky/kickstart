#!/usr/bin/env bash

set -euf

make
clear

RUN=""
if [ "$1" = "-g" ]; then
    RUN="valgrind"
    shift
fi

echo ===== hack "$@" =====
$RUN ./hack "$@"
