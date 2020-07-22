# Compiler, gcc or clang
COMPILER := gcc
CPP_STANDARD := c++17

# Compiler command
CC := g++

CODE := ./code.cpp
EXE := ./hack

include ../../../templates/cpp-flags.make

# * Modes

.PHONY: perfm debug build run valgrind clean

build: debug

perfm: MODE := performance
perfm: CXXFLAGS += $(OPTIM)
perfm: $(EXE)

debug: MODE := debug
debug: CXXFLAGS += $(DEBUG)
debug: $(EXE)

# Compilation rule
$(EXE): $(CODE)
	@echo "CC (${COMPILER} $(MODE))  $@"
	@$(CXX) $(CODE) $(CXXFLAGS) -o $@

run: build
	@$(EXE)

valgrind: build
	@clear
	@valgrind --leak-check=full --show-leak-kinds=all $(EXE)

clean:
	@rm -f $(EXE) core vgcore.* *.log
