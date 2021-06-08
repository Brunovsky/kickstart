#!/usr/bin/env bash

set -eu

declare -r CASE='Case #'
declare -r TRACE='::hack '
declare -r VALGRIND=(valgrind)

declare -r PROG_NAME=$(basename "$0")
declare -r ROOT=$(git rev-parse --show-cdup)
declare -r ACTION="${1:-run}"
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
			grep -svP "$TRACE" "$input" | "${VALGRIND[@]}" ./solver | tee output.txt
			if cmp "$output" "output.txt"; then
				echo "$input OK"
			else
				diff -y --minimal "$output" "output.txt" || true
			fi
		fi
	done
}

function main {
	case "$ACTION" in
		*help*)
			echo "Usage: $PROG_NAME action [args]..." >&2
		;;
		# Pure make commands
		clean)
			run_make clean
		;;
		make|build|debug)
			run_make debug
		;;
		perfm)
			run_make perfm
		;;
		rebuild)
			run_make clean && run_make debug
		;;
		# Make and run commands
		in*)
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
		# Make and run under valgrind
		valg|valgrind|valgfast)
			run_make_solver
			grep -svP "$TRACE" input.txt | "${VALGRIND[@]}" ./solver | tee output.txt
		;;
		valgplain*)
			run_make_solver
			"${VALGRIND[@]}" ./solver | tee output.txt
		;;
		valgtest*)
			run_make_solver
			run_valgrind_tests
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
