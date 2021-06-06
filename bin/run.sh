#!/usr/bin/env bash

declare -r CASE='Case #'
declare -r TRACE='::hack '
declare -r VALGRIND=(valgrind)

declare -r ROOT=$(git rev-parse --show-cdup)
declare -r COMMAND="$1"
shift

function run_make { make -f "$ROOT/common/Makefile" "$@" ; }

function run_make_solver {
	if [[ "$COMMAND" == *fast* || "$COMMAND" == *perfm* ]]; then
		run_make perfm "$@"
	else
		run_make debug "$@"
	fi
}

function run_input {
	if test -f input.txt; then
		grep -svP "$TRACE" input.txt | ./solver | tee output.txt
	else
		./solver | tee output.txt
	fi
}

function run_plain {
	"$@" ./solver | tee output.txt
}

function run_tests {
	for input in *.in; do
		output=${input%in}out
		if test -f "$output"; then
			echo "$input" "$output"
		fi
	done
}

function main {
	case "$COMMAND" in
		*help*)
			echo "Usage: $0 command [args]..." >&2
		;;
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
		# Run against input.txt if it exists, or standalone if it doesn't
		run|fast)
			run_make_solver
			run_input
		;;
		plain*|standalone)
			run_make_solver
			run_plain
		;;
		test*)
			run_make_solver
			run_tests
		;;
		# Run under valgrind
		valg|valgrind|valgfast)
			run_make_solver
			run_input "${VALGRIND[@]}" "$@"
		;;
		valgplain*)
			run_make_solver
			run_plain "${VALGRIND[@]}" "$@"
		;;
		valgtest*)
			run_make_solver
			run_tests "${VALGRIND[@]}" "$@"
		;;
		# Run interactive with judge
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
	esac
}

test "${BASH_SOURCE[0]}" != "${0}" || main "$@"
