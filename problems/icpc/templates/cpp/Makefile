# Compiler, gcc or clang
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt -Wno-keyword-macro
TRACE := '::hack '

include ../../../common/cxx.make

.PHONY: run fast valgrind hack int fastint

run: debug
	@grep -svP $(TRACE) input.txt | ./solver | tee output.txt

fast: perfm
	@grep -svP $(TRACE) input.txt | ./solver | tee output.txt

valgrind: debug
	@grep -svP $(TRACE) input.txt | $(VALGRIND) ./solver | tee output.txt

hack: hacker
	@./hacker > output.txt && wc output.txt

int: debug judge
	@run.sh --from-make

fastint: perfm judge
	@run.sh --from-make
