#!/bin/bash

read -p "Competition?  " COMPETITION_NAME
[[ -z "$COMPETITION_NAME" ]] && COMPETITION_NAME=all

read -p "Problem name? " PROBLEM_NAME
[[ -z "$PROBLEM_NAME" ]] && exit 0

problem_folder="problems/$COMPETITION_NAME/$PROBLEM_NAME"

mkdir -p "problems/$COMPETITION_NAME"
cp -Rn "$TEMPLATE" problem_folder
echo "# $COMPETITION_NAME - $PROBLEM_NAME" > problem_folder/README.md
