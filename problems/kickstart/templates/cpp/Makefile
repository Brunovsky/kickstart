# Compiler, gcc or clang
COMPILER := gcc
CPP_STANDARD := c++17

# Compiler command
CC := g++

CODE := ./code.cpp
NAIVE := ./naive.cpp
GEN := ./generate.cpp

EXE := ./hack
NAIVEEXE := ./hack_naive
GENEXE := ./hack_generate

include ../../../../templates/cpp-flags.make

# * Modes

.PHONY: perfm debug build_naive build_generate
.PHONY: run code valgrind fast test naive generate clean

perfm: MODE := performance
perfm: CXXFLAGS += $(OPTIM)
perfm: $(EXE)

debug: MODE := debug
debug: CXXFLAGS += $(DEBUG)
debug: $(EXE)

build_naive: MODE := performance
build_naive: CXXFLAGS += $(OPTIM)
build_naive: $(NAIVEEXE)

build_generate: MODE := performance
build_generate: CXXFLAGS += $(OPTIM)
build_generate: $(GENEXE)

$(EXE): $(CODE)
	@echo "CC (${COMPILER} $(MODE))  $@"
	@$(CXX) $(CODE) $(CXXFLAGS) -o $@

$(NAIVEEXE): $(NAIVE)
	@echo "CC (${COMPILER} $(MODE))  $@"
	@$(CXX) $(NAIVE) $(CXXFLAGS) -o $@

$(GENEXE): $(GEN)
	@echo "CC (${COMPILER} $(MODE))  $@"
	@$(CXX) $(GEN) $(CXXFLAGS) -o $@

# Compile and run, build in debug mode by default
run code: debug
	@$(EXE) < input.txt

# Build in debug mode and run under valgrind
valgrind: debug
	@clear
	@valgrind $(EXE) < input.txt

# Compile and run in perfm mode
fast: perfm
	@$(EXE) < input.txt

# Compile, run and test against the naive's version, build in perfm mode by default
test: perfm
	@$(EXE) < input.txt > program.txt
	@cmp program.txt naive.txt && echo OK

# Compile and run the naive version in 'naive.cpp', output to naive.txt
naive: build_naive
	@$(NAIVEEXE) < input.txt > naive.txt

# Compile and run the input generator in 'generate.cpp', output directly to input.txt
generate: build_generate
	@$(GENEXE) > input.txt

clean:
	@rm -f $(EXE) $(NAIVEEXE) $(GENEXE) core vgcore.* *.log
