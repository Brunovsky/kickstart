#!/bin/bash

read -p "Year? " YEAR
[[ -z "$YEAR" ]] && exit 0

COMPETITION="kickstart-$YEAR"

read -p "Round? " ROUND
[[ -z "$ROUND" ]] && exit 0

read -p "Problem name? " NAME
[[ -z "$NAME" ]] && exit 0

PROBLEM="$ROUND-$NAME"
PROBLEM_FOLDER="problems/$COMPETITION/$PROBLEM"

mkdir -p "problems/$COMPETITION"
cp -RnT "$TEMPLATE" "$PROBLEM_FOLDER"
echo "# $COMPETITION - $PROBLEM" > "$PROBLEM_FOLDER/README.md"
cd "$PROBLEM_FOLDER"
code .
