# Compiler, gcc or clang
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt

.PHONY: default-target
default-target: debug

# * Files

CODE := code.cpp
GENERATE := generate.cpp

RUN := ./run.sh
EXE := ./hack
GENERATE_EXE := ./hack_generate

# * Modes

include ../../../../common/cxxflags.make
CXXFLAGS += $(EXTRA_CXXFLAGS)

.PHONY: perfm debug clean

debug: MODE := debug
debug: CXXFLAGS += $(DEBUG)
debug: $(EXE)

perfm: MODE := performance
perfm: CXXFLAGS += $(OPTIM)
perfm: $(EXE)

clean:
	@rm -f $(EXE) core vgcore.* *.log

$(EXE): $(CODE)
	@echo "CC (${COMPILER} $(MODE))  $@"
	@$(CXX) $(CODE) $(CXXFLAGS) -o $@

# ---

.PHONY: run fast

run: debug
	@+$(RUN) --from-make

fast: perfm
	@+$(RUN) --from-make
