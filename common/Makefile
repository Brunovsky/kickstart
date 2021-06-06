# C++ template
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt
TRACE := '::hack '

export GIT_ROOT := $(shell git rev-parse --show-cdup)
include $(GIT_ROOT)common/cxxcore.make

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
.PHONY: casediff linediff

casediff:
	@casediff program.txt correct.txt $(ARGS)

sidediff:
	@diff -y --tabsize=4 --minimal program.txt correct.txt $(ARGS)

diff:
	@diff --unchanged-line-format="" \
		  --old-line-format="< %dn: %L" \
		  --new-line-format="> %dn: %L" \
		  --minimal program.txt correct.txt $(ARGS)
