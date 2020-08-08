# Compiler, gcc or clang
COMPILER := gcc
CPP_STANDARD := c++17

# Compiler command
CC := g++

CODE := ./code.cpp
RUN := ./run.sh

EXE := ./hack
LIBS := -lfmt

include ../../../templates/cpp-flags.make
CXXFLAGS += $(LIBS)

# * Modes

.PHONY: perfm debug
.PHONY: run code valgrind fast clean

all: debug
	@true

perfm: MODE := performance
perfm: CXXFLAGS += $(OPTIM)
perfm: $(EXE)

debug: MODE := debug
debug: CXXFLAGS += $(DEBUG)
debug: $(EXE)

$(EXE): $(CODE)
	@echo "CC (${COMPILER} $(MODE))  $@"
	@$(CXX) $(CODE) $(CXXFLAGS) -o $@

# Compile and run, build in debug mode by default
run code: debug
	@$(RUN)

fast: perfm
	@$(RUN)

# Build in debug mode and run under valgrind
valgrind: debug
	@clear
	@$(RUN) -g

clean:
	@rm -f $(EXE) core vgcore.* *.log
