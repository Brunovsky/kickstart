# C++ template
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt -Wno-keyword-macro
TRACE := '::hack '

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


# Standard C++ problems with an input file
.PHONY: run fast valgrind

run: debug
	@grep -svP $(TRACE) input.txt | ./solver | tee output.txt

fast: perfm
	@grep -svP $(TRACE) input.txt | ./solver | tee output.txt

valgrind: debug
	@grep -svP $(TRACE) input.txt | $(VALGRIND) ./solver | tee output.txt

# Standard C++ problems without an input file (e.g. testing with kbd input)
.PHONY: plainrun plainfast plainvalgrind

plain: debug
	@./solver | tee output.txt

plainfast: fast
	@./solver | tee output.txt

plainvalgrind: debug
	@$(VALGRIND) ./solver | tee output.txt

# Interactive problems with a python judge
.PHONY: intpy fastintpy

intpy: debug
	python3 interactive_runner.py ./judge.py $(ARGS) -- ./solver

fastintpy: perfm
	python3 interactive_runner.py ./judge.py $(ARGS) -- ./solver

# Interactive problems with a C++ judge
.PHONY: intcxx fastintcxx

intcxx: debug ./judger
	python3 interactive_runner.py ./judger $(ARGS) -- ./solver

fastintcxx: fast ./judger
	python3 interactive_runner.py ./judger $(ARGS) -- ./solver

# Interactive problems with a testing tool
.PHONY: inttest fastinttest

inttest: debug ./judger
	python3 ./testing_tool.py $(ARGS) ./solver

fastinttest: fast ./judger
	python3 ./testing_tool.py $(ARGS) ./solver

# Other stuff
.PHONY: diff linediff

diff:
	@casediff program.txt correct.txt $(ARGS)

linediff:
	@diff -y --tabsize=4 --minimal program.txt correct.txt $(ARGS)
