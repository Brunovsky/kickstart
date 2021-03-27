# Compiler, gcc or clang
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt

.PHONY: default-target
default-target: debug

# * Files

CODE := ./code.cpp
NAIVE := ./naive.cpp
GENERATE := ./generate.cpp

EXE := ./hack
NAIVE_EXE := ./hack_naive
GENERATE_EXE := ./hack_generate

# * Modes

include ../../../../common/cxxflags.make
CXXFLAGS += $(EXTRA_CXXFLAGS)

.PHONY: debug perfm build_naive build_generate clean

debug: MODE := debug
debug: CXXFLAGS += $(DEBUG)
debug: $(EXE)

perfm: MODE := performance
perfm: CXXFLAGS += $(OPTIM)
perfm: $(EXE)

build_naive: MODE := performance
build_naive: CXXFLAGS += $(OPTIM)
build_naive: $(NAIVE_EXE)

build_generate: MODE := performance
build_generate: CXXFLAGS += $(OPTIM)
build_generate: $(GENERATE_EXE)

clean:
	@rm -f $(EXE) $(NAIVE_EXE) $(GENERATE_EXE) core vgcore.* *.log program.txt

$(EXE): $(CODE)
	@echo "CC (${COMPILER} $(MODE))  $@"
	@$(CXX) $(CODE) $(CXXFLAGS) -o $@

$(NAIVE_EXE): $(NAIVE)
	@echo "CC (${COMPILER} $(MODE))  $@"
	@$(CXX) $(NAIVE) $(CXXFLAGS) -o $@

$(GENERATE_EXE): $(GENERATE)
	@echo "CC (${COMPILER} $(MODE))  $@"
	@$(CXX) $(GENERATE) $(CXXFLAGS) -o $@

# ---

.PHONY: run fast valgrind naive generate

# Compile and run, build in debug mode by default, output to program.txt
run: debug
	@$(EXE) < input.txt | tee program.txt

# Compile and run with optimization flags, output to program.txt
fast: perfm
	@$(EXE) < input.txt | tee program.txt

# Build in debug mode and run under valgrind, output to debug.txt
valgrind: debug
	@$(VALGRIND) $(EXE) < input.txt | tee debug.txt

# Compile and run the naive version in 'naive.cpp', output to correct.txt
naive: build_naive
	@$(NAIVE_EXE) < input.txt | tee correct.txt

# Compile and run the input generator in 'generate.cpp', output directly to input.txt
generate: build_generate
	@$(GENERATE_EXE) > input.txt

# ---

.PHONY: diff linediff

# Compare program.txt with correct.txt byte-by-byte
diff:
	@python ../../../../common/casediff.py program.txt correct.txt

# Compare program.txt with correct.txt line-by-line and output differing lines
linediff:
	@diff -y --tabsize=4 --minimal program.txt correct.txt
