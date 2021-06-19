#!/usr/bin/env bash

set -eu

declare -r CASE='Case #'
declare -r TRACE='::hack'
declare -r VALGRIND=(valgrind)

declare -r PROG_NAME=$(basename "$0")
declare -r ROOT=$(git rev-parse --show-cdup)
declare -r ACTION="${1:-run}"
declare -r TEMPLATES="$ROOT/templates"
if test $# -gt 0; then shift; fi

function run_make {
	if test -f Makefile; then # use the Makefile in pwd if there is one
		make "$@"
	else
		make -sf "$ROOT/common/Makefile" "$@"
	fi
}

function run_make_solver {
	if [[ "$ACTION" == *fast* || "$ACTION" == *perfm* ]]; then
		run_make perfm "$@"
	else
		run_make debug "$@"
	fi
}

function run_tests {
	for input in *.in; do
		output=${input%in}out
		if test -f "$output"; then
			grep -svP "$TRACE" "$input" | ./solver > output.txt
			if cmp "$output" "output.txt"; then
				echo "$input OK"
			else
				diff -y --minimal "$output" "output.txt" || true
			fi
		fi
	done
}

function run_valgrind_tests {
	for input in *.in; do
		output=${input%in}out
		if test -f "$output"; then
			grep -svP "$TRACE" "$input" | "${VALGRIND[@]}" "$@" ./solver | tee output.txt
			if cmp "$output" "output.txt"; then
				echo "$input OK"
			else
				diff -y --minimal "$output" "output.txt" || true
			fi
		fi
	done
}

# Ugly as fuck, but if it works it ain't broken
function main {
	case "$ACTION" in
		*help*)
			echo "Usage: $PROG_NAME action [args]..." >&2
		;;
		load)
			if test $# -gt 0 -a -d "$TEMPLATES/$1"; then
				cp "$TEMPLATES/$1/"* -r .
			else
				echo "'$1' template is not defined or does not exist"
			fi
		;;
		# Pure make commands
		clean|debug|perfm)
			run_make "$ACTION"
		;;
		make)
			run_make debug
		;;
		rebuild|remake)
			run_make clean && run_make debug
		;;
		# Make and run under valgrind
		*invalg*|*valgin*|*valgrindin*)
			run_make_solver
			echo "grep -svP "$TRACE" input.txt | "${VALGRIND[@]}" "$@" ./solver | tee output.txt"
			grep -svP "$TRACE" input.txt | "${VALGRIND[@]}" "$@" ./solver | tee output.txt
		;;
		*testvalg*|*valgtest*|*valgrindtest*)
			run_make_solver
			echo run_valgrind_tests "$@"
			run_valgrind_tests "$@"
		;;
		*valg*)
			run_make_solver
			"${VALGRIND[@]}" "$@" ./solver
		;;
		# Make and run commands
		in*|fastin*|infast*)
			run_make_solver
			grep -svP "$TRACE" input.txt | ./solver | tee output.txt
		;;
		run*|fast*)
			run_make_solver
			./solver
		;;
		test*)
			run_make_solver
			run_tests "$@"
		;;
		# Run interactive with judge
		judgepy*)
			run_make_solver
			interactive_runner ./judge.py "$@" -- ./solver
		;;
		judge*)
			run_make_solver
			interactive_runner ./judge "$@" -- ./solver
		;;
		# Run the hacker
		hack*)
			run_make_solver hacker
			pipehack
		;;
		gen*)
			run_make hacker
			./hacker "$@" > input.txt
		;;
		*)
			echo "Unknown action '$ACTION'"
			exit 1
		;;
	esac
}

test "${BASH_SOURCE[0]}" != "${0}" || main "$@"
