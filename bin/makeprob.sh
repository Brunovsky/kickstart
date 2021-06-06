#!/usr/bin/env bash

set -euf

declare -r ROOT=$(git rev-parse --show-toplevel)
declare -r FOLDER="$1"
declare -r TMPL="$ROOT/templates/$2"

if test -e "$FOLDER"; then
	echo "makeprob: File $1 exists" >&2
fi
if test ! -d "$TMPL"; then
	echo "makeprob: Template $2 not found" >&2
	exit 2
fi

cp -r "$TMPL" "$FOLDER"
