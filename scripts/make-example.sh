#!/bin/bash

read -p "Name? (folder) " NAME
[[ -z "$NAME" ]] && exit 0

COMPETITION="examples/$YEAR"

HEADER="Example $NAME"
PROBLEM="$NAME"
PROBLEM_FOLDER="problems/examples/$PROBLEM"

mkdir -p "$PROBLEM_FOLDER"
cp -RnT "$TEMPLATE" "$PROBLEM_FOLDER"

echo "# $HEADER" > "$PROBLEM_FOLDER/README.md"
ln -s -T "$PWD/hacktools" "$PWD/$PROBLEM_FOLDER/lib"

cd "$PROBLEM_FOLDER"
code .
