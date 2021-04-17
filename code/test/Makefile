# Compiler, gcc or clang
COMPILER := gcc
CC := g++
CPP_STANDARD := c++17
EXTRA_CXXFLAGS := -lfmt -Wno-sign-compare

BIN_DIR := ./bin
DEP_DIR := ./dep

HEADERS := $(shell find .. -name '*.hpp')
CXX_FILES := $(wildcard *.cpp)
CXX_FILES := $(filter-out $(FILE), $(CXX_FILES))
EXE_FILES := $(CXX_FILES:%.cpp=${BIN_DIR}/%)
DEP_FILES := $(CXX_FILES:%.cpp=${DEP_DIR}/%.d)

include ../../common/cxx.make
CXXFLAGS += $(EXTRA_CXXFLAGS)

-include $(DEP_FILES)

# global build

CMD_MKDIRS = @mkdir -p $(BIN_DIR) $(DEP_DIR)
CMD_MV_DEPENDENCY = @mv -f $(DEP_DIR)/$*.Td $(DEP_DIR)/$*.d && touch $@

DEPCOMPILEARGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.Td

$(DEP_DIR)/%.d: ;
.PRECIOUS: $(DEP_DIR)/%.d

$(EXE_FILES): $(BIN_DIR)/%: %.cpp $(DEP_DIR)/%.d
	@echo "CC (${COMPILER} ${MODE})  $<"
	@$(CMD_MKDIRS)
	@$(CXX) $< $(CXXFLAGS) $(DEPCOMPILEARGS) -o $@
	@$(CMD_MV_DEPENDENCY)

.PHONY: all cleanall

all: MODE := debug
all: CXXFLAGS += $(DEBUG)
all: $(EXE_FILES)

allperfm: MODE := perfm
allperfm: CXXFLAGS += $(OPTIM)
allperfm: $(EXE_FILES)

cleanall: clean
	@rm -rf $(DEP_DIR) $(BIN_DIR)

# ---

$(SOLVER): $(HEADERS)

run: debug
	@$(SOLVER)

fast: perfm
	@$(SOLVER)

valgrind: debug
	@$(VALGRIND) $(SOLVER)
