# Compiler, gcc or clang
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt

include ../../../../common/cxx.make

run: debug
	@+$(RUN) --from-make

fast: perfm
	@+$(RUN) --from-make
