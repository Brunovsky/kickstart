# Compiler, gcc or clang
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt

.PHONY: default-target
default-target: debug

# * Files

CODE := ./code.cpp
RUN := ./run.sh

EXE := ./hack

# * Modes

include ../../../common/cxxflags.make
CXXFLAGS += $(EXTRA_CXXFLAGS)

.PHONY: perfm debug build_generate
.PHONY: run code valgrind fast clean

perfm: MODE := performance
perfm: CXXFLAGS += $(OPTIM)
perfm: $(EXE)

debug: MODE := debug
debug: CXXFLAGS += $(DEBUG)
debug: $(EXE)

build_generate: MODE := performance
build_generate: CXXFLAGS += $(OPTIM)
build_generate: $(GENERATE_EXE)

clean:
	@rm -f $(EXE) $(GENERATE_EXE) core vgcore.* *.log

$(EXE): $(CODE)
	@echo "CC (${COMPILER} $(MODE))  $@"
	@$(CXX) $(CODE) $(CXXFLAGS) -o $@

# ---

.PHONY: run fast valgrind generate

run: debug
	@$(RUN)

fast: perfm
	@$(RUN)

valgrind: debug
	@$(RUN) -g

generate: build_generate
	@$(GENERATE_EXE) > input.txt

stats:
	@$(RUN) --stats
