# Compiler, gcc or clang
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt
TRACE := '::hack '

include ../../../common/cxx.make

.PHONY: run fast valgrind

# Compile and run with debug flags
run: debug
	@grep -svP $(TRACE) input.txt | ./solver | tee output.txt

# Compile and run, optimized
fast: perfm
	@grep -svP $(TRACE) input.txt | time ./solver | tee output.txt

# Build and run under valgrind
valgrind: debug
	@grep -svP $(TRACE) input.txt | $(VALGRIND) ./solver | tee output.txt
