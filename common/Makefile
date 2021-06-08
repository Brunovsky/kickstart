# C++ template
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt

export GIT_ROOT := $(shell git rev-parse --show-cdup)
export CXX := $(CC) -std=$(CPP_STANDARD) -pipe -pthread -pedantic -march=native
export VALGRIND := valgrind --show-leak-kinds=all

export USE_CLANG_LIBCPP := -stdlib=libc++
export USE_GLIBS_DEBUG := -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC

include $(GIT_ROOT)common/cxxwarns.make

export CXXFLAGS := $(WARNS) $(CXXFLAGS) $(EXTRA_CXXFLAGS)
export OPTIM := -O3 -flto -funroll-loops -ftree-vectorize
export DEBUG := -g -Og -ggdb $(USE_GLIBS_DEBUG)

.PHONY: debug perfm clean dry-debug dry-perfm

debug: MODE := debug
debug: CXXFLAGS += $(DEBUG)
debug: ./solver

perfm: MODE := perfm
perfm: CXXFLAGS += $(OPTIM)
perfm: ./solver

clean::
	@rm -f solver hacker judger core vgcore.* *.log output.txt answer.txt

./solver: code.cpp
	@echo CC ${COMPILER} ${MODE} $@
	@$(CXX) code.cpp $(CXXFLAGS) -o $@

./hacker: hack.cpp
	@echo CC ${COMPILER} $@
	@$(CXX) hack.cpp $(CXXFLAGS) $(OPTIM) -o $@

./judger: judge.cpp
	@echo CC ${COMPILER} $@
	@$(CXX) judge.cpp $(CXXFLAGS) $(OPTIM) -o $@