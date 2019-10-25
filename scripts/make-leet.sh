#!/bin/bash

read -p "Difficulty? " DIFFICULTY
[[ -z "$DIFFICULTY" ]] && exit 0

COMPETITION="leet/$DIFFICULTY"

read -p "Problem name? " NAME
[[ -z "$NAME" ]] && exit 0

PROBLEM="$NAME"
HEADER="LeetCode - $DIFFICULTY - $PROBLEM"
PROBLEM_FOLDER="problems/$COMPETITION/$PROBLEM"

mkdir -p "problems/$COMPETITION"
mkdir -p "problems/$COMPETITION/$PROBLEM_FOLDER"
cp -RnT "$TEMPLATE" "$PROBLEM_FOLDER"
echo "# $HEADER" > "$PROBLEM_FOLDER/README.md"
cd "$PROBLEM_FOLDER"
code .
