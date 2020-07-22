# Compiler, gcc or clang
COMPILER := gcc
CPP_STANDARD := c++14

# Compiler command
CC := g++

CODE := code.cpp
GEN := generate.cpp

RUN := ./run.sh
EXE := ./hack
GENEXE := ./hack_generate

include ../../../../templates/cpp-flags.make

# * Modes

.PHONY: perfm debug build_naive build_generate
.PHONY: run code fast generate clean

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
	@$(RUN)

# Compile and run in perfm mode
fast: perfm
	@$(RUN)

# Compile and run the input generator in 'generate.cpp'
generate: build_generate
	@$(GENEXE) > input.txt

clean:
	@rm -f $(EXE) $(NAIVEEXE) $(GENEXE) core vgcore.* *.log
