#!/bin/bash

read -p "Competition?  " COMPETITION_NAME
[[ -z "$COMPETITION_NAME" ]] && COMPETITION_NAME=all

read -p "Problem name? " PROBLEM_NAME
[[ -z "$PROBLEM_NAME" ]] && exit 0

PROBLEM_FOLDER="problems/$COMPETITION_NAME/$PROBLEM_NAME"

mkdir -p "problems/$COMPETITION_NAME"
cp -RnT "$TEMPLATE" "$PROBLEM_FOLDER"
echo "# $COMPETITION_NAME - $PROBLEM_NAME" > "$PROBLEM_FOLDER/README.md"
