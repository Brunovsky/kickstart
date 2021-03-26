# Compiler, gcc or clang
COMPILER := gcc
CPP_STANDARD := c++17
LIBS := -lfmt

default-target: debug

# Compiler command
CC := g++

FILE := code.cpp
EXE := ./hack
BIN_DIR := ./bin
DEP_DIR := ./dep

HEADERS := $(shell find .. -name '*.hpp')
CXX_FILES := $(wildcard *.cpp)
CXX_FILES := $(filter-out $(FILE), $(CXX_FILES))
EXE_FILES := $(CXX_FILES:%.cpp=${BIN_DIR}/%)
DEP_FILES := $(CXX_FILES:%.cpp=${DEP_DIR}/%.d)

include ../../templates/cpp-flags.make
CXXFLAGS += $(LIBS) -Wno-sign-compare
SANIT := $(THREAD_SANIT)

-include $(DEP_FILES)

# * Modes

.PHONY: perfm debug all
.PHONY: run code valgrind fast clean

debug: MODE := debug
debug: CXXFLAGS += $(DEBUG)
debug: $(EXE)

perfm: MODE := performance
perfm: CXXFLAGS += $(OPTIM)
perfm: $(EXE)

sanit: MODE := sanitizer
sanit: CXXFLAGS += $(DEBUG) $(SANIT)
sanit: $(EXE)

prof: MODE := profiling
prof: CXXFLAGS += $(OPTIM) -pg
prof: $(EXE)

fastdebug: MODE := fastdebug
fastdebug: CXXFLAGS += $(DEBUG) $(OPTIM) -UNDEBUG
fastdebug: $(EXE)

# global build

CMD_MKDIRS = @mkdir -p $(BIN_DIR) $(DEP_DIR)
CMD_MV_DEPENDENCY = @mv -f $(DEP_DIR)/$*.Td $(DEP_DIR)/$*.d && touch $@

DEPCOMPILEARGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.Td

$(DEP_DIR)/%.d: ;
.PRECIOUS: $(DEP_DIR)/%.d

$(EXE_FILES): MODE := debug
$(EXE_FILES): CXXFLAGS += $(DEBUG)
$(EXE_FILES): $(BIN_DIR)/%: %.cpp $(DEP_DIR)/%.d
	@echo "CC (${COMPILER} ${MODE})  $<"
	@$(CMD_MKDIRS)
	@$(CXX) $< $(CXXFLAGS) $(DEPCOMPILEARGS) -o $@
	@$(CMD_MV_DEPENDENCY)

all: $(EXE_FILES)

cleanall: clean
	@rm -rf $(DEP_DIR) $(BIN_DIR)

# code.cpp

$(EXE): $(FILE) $(HEADERS)
	@echo "CC (${COMPILER} $(MODE)) $@"
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
