#!/bin/bash

read -p "Year? " YEAR
[[ -z $YEAR ]] && exit 0
[[ $YEAR =~ '20[0-9]{2}' ]] || echo 'Bad year' && exit 1

read -p "Round? " ROUND
[[ -z $ROUND ]] && exit 0
[[ $ROUND =~ '[A-Z]' ]] || echo 'Bad round' && exit 1

read -p "Problem name? " NAME
[[ -z $NAME ]] && exit 0
[[ $NAME =~ '[a-zA-Z0-9-]+' ]] || echo 'Bad problem name' && exit 1

COMPETITION="kickstart-$YEAR"
PROBLEM="$ROUND-$NAME"
HEADER="Kickstart $YEAR - $ROUND-$NAME"
FOLDER="problems/$COMPETITION/$PROBLEM"

mkdir -p "problems/$COMPETITION"
cp -RnT "$TEMPLATE" "$FOLDER"

echo "# $HEADER" > "$FOLDER/README.md"

cd "$FOLDER"
${EDITOR:-code} .
