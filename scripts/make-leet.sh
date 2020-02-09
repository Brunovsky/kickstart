#!/bin/bash

read -p "Difficulty? " DIFFICULT
[[ -z $DIFFICULTY ]] && exit 0
[[ $DIFFICULTY =~ '[a-z]+' ]] || echo 'Bad difficulty' && exit 1

read -p "Problem name? " NAME
[[ -z $NAME ]] && exit 0
[[ $NAME =~ '[a-zA-Z0-9-]+' ]] || echo 'Bad problem name' && exit 1

COMPETITION="leet/$DIFFICULTY"
HEADER="LeetCode - $DIFFICULTY - $NAME"
PROBLEM="$NAME"
FOLDER="problems/$COMPETITION/$PROBLEM"

mkdir -p "$FOLDER"
cp -RnT "$TEMPLATE" "$FOLDER"

echo "# $HEADER" > "$FOLDER/README.md"

cd "$FOLDER"
${EDITOR:-code} .
