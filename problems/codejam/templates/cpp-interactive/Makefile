# Compiler, gcc or clang
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt

include ../../../../common/cxx.make

run: debug
	@./run.sh --from-make

fast: perfm
	@./run.sh --from-make
