# Compiler, gcc or clang
COMPILER := clang
CC := clang++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt -Wno-keyword-macro
TRACE := '::hack '

include ../../../common/cxx.make

.PHONY: run fast valgrind

# Compile and run with debug flags
run: debug
	@grep -svP $(TRACE) input.txt | ./solver | tee output.txt

# Compile and run, optimized
fast: perfm
	@grep -svP $(TRACE) input.txt | ./solver | tee output.txt

# Build and run under valgrind
valgrind: debug
	@grep -svP $(TRACE) input.txt | $(VALGRIND) ./solver | tee output.txt
