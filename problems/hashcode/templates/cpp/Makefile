# Compiler, gcc or clang
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt
RUN := ./run.sh
TRACE := '::hack '

include ../../../../common/cxx.make

.PHONY: run fast valgrind

run: debug
	@$(RUN)

fast: perfm
	@$(RUN)

valgrind: debug
	@$(RUN) -g
