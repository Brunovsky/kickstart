# Compiler, gcc or clang
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt

include ../../../common/cxx.make

.PHONY: run fast valgrind

# Compile and run with debug flags
run: debug
	./solver | tee output.txt

# Compile and run, optimized
fast: perfm
	time ./solver | tee output.txt

# Build and run under valgrind
valgrind: debug
	$(VALGRIND) ./solver | tee output.txt
