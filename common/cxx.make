# C++ template

export CXX := $(CC) -std=$(CPP_STANDARD) -pipe -pthread -pedantic -march=native
export VALGRIND := valgrind --show-leak-kinds=all

export USE_CLANG_LIBCPP := -stdlib=libc++
export USE_GLIBS_DEBUG := -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC

include $(dir $(lastword $(MAKEFILE_LIST)))cxxwarns.make
export CXXFLAGS += $(WARNS) $(EXTRA_CXXFLAGS)
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
	@rm -f solver hacker core vgcore.* *.log output.txt answer.txt

./hacker: hack.cpp
	@echo CC ${COMPILER} $@
	@$(CXX) hack.cpp $(CXXFLAGS) $(OPTIM) -o $@

./solver: code.cpp
	@echo CC ${COMPILER} ${MODE} $@
	@$(CXX) code.cpp $(CXXFLAGS) -o $@
