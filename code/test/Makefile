# Compiler, gcc or clang
COMPILER := clang
CPP_STANDARD := c++14

# Compiler command
CC := clang++

FILE := code.cpp
EXE := ./hack

DEPS := $(shell find .. -name '*.hpp')
LIBS := -lfmt

include ../../templates/cpp-flags.make
CXXFLAGS += $(LIBS)

# * Modes

.PHONY: perfm debug
.PHONY: run code valgrind fast clean

debug: MODE := debug
debug: CXXFLAGS += $(DEBUG)
debug: $(EXE)

perfm: MODE := performance
perfm: CXXFLAGS += $(OPTIM)
perfm: $(EXE)

# Compilation rule
$(EXE): $(FILE) $(DEPS)
	@echo "CC (${COMPILER} $(MODE))  $@"
	@$(CXX) $(FILE) $(CXXFLAGS) -o $@

run code: debug
	@$(EXE)

valgrind: debug
	@clear
	@valgrind --leak-check=full $(EXE)

fast: perfm
	@$(EXE)

clean:
	@rm -f $(EXE) core vgcore.* *.log
