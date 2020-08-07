#!/usr/bin/env bash

make
echo ===== hack "$@" =====
./hack "$@"
