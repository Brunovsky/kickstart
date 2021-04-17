# C++ template
# We use make to avoid bothering with completions and update-to-date compilations

COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt

.PHONY: default-target
default-target: debug

SOLVER_CPP := ./code.cpp
HACKER_CPP := ./hack.cpp
SOLVER := ./solver
HACKER := ./hacker
INPUT  := input.txt
OUTPUT := output.txt
ANSWER := answer.txt
PATTERN := 'CASE \#(\d+):'
TRACE   := '::hack '

# * Compilation options

CXX := $(CC) -std=$(CPP_STANDARD) -pipe -pthread -pedantic -march=native
VALGRIND := valgrind --show-leak-kinds=all

USE_CLANG_LIBCPP := -stdlib=libc++
USE_GLIBS_DEBUG := -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC

include $(dir $(lastword $(MAKEFILE_LIST)))cxxwarns.make
CXXFLAGS := $(WARNS) $(EXTRA_CXXFLAGS)
OPTIM := -O3 -flto -funroll-loops -ftree-vectorize
DEBUG := -g -Og -ggdb $(USE_GLIBS_DEBUG)

# ---

.PHONY: debug perfm clean

debug: MODE := debug
debug: CXXFLAGS += $(DEBUG)
debug: $(SOLVER)

perfm: MODE := perfm
perfm: CXXFLAGS += $(OPTIM)
perfm: $(SOLVER)

clean:
	@rm -f $(SOLVER) $(HACKER) $(OUTPUT) core vgcore.* *.log $(ANSWER)

$(HACKER): $(HACKER_CPP)
	@echo CC ${COMPILER} $@
	@$(CXX) $(HACKER_CPP) $(CXXFLAGS) $(OPTIM) -o $@

$(SOLVER): $(SOLVER_CPP)
	@echo CC ${COMPILER} ${MODE} $@
	@$(CXX) $(SOLVER_CPP) $(CXXFLAGS) -o $@


# ---

.PHONY: run fast valgrind hack pipe diff

# Compile and run with debug flags
run-cxx: debug
	@grep -vP $(TRACE) $(INPUT) | $(SOLVER) | tee $(OUTPUT)

# Compile and run, optimized
fast-cxx: perfm
	@grep -vP $(TRACE) $(INPUT) | time $(SOLVER) | tee $(OUTPUT)

# Build and run under valgrind
valgrind-cxx: debug
	@grep -vP $(TRACE) $(INPUT) | $(VALGRIND) $(SOLVER) | tee $(OUTPUT)

# Run the hacker (input generator)
hack-cxx: $(HACKER)
	@$(HACKER) $(ARGS) > $(INPUT)

# Run pipehack (in bin)
pipe-cxx: perfm $(HACKER)
	@pipehack $(ARGS)

# Run casediff (in bin)
diff-cxx:
	@casediff $(OUTPUT) $(ANSWER) $(ARGS)

%: %-cxx
