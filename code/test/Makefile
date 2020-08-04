# Compiler, gcc or clang
COMPILER := gcc
CPP_STANDARD := c++14

# Compiler command
CC := g++

FILE := code.cpp
EXE := ./hack

DEPS := ../disjoint_set.hpp ../fenwick.hpp ../quad_tree.hpp ../segment_tree.hpp
DEPS += ../russian_segment_tree.hpp ../maximal_matching.hpp ../maximum_flow.hpp
DEPS += ../mincost_matching.hpp ../simplex.hpp ../shortest_path.hpp
DEPS += ../min_spanning_forest.hpp ../transportation.hpp ../linear_system.hpp
DEPS += ../hash.hpp ../math.hpp ../sieve.hpp ../bits.hpp ../frac.hpp
DEPS += ../kmp.hpp ../dancing_links.hpp ../avl_tree.hpp ../bs_tree.hpp
DEPS += ../bs_set.hpp ../bs_map.hpp ../rb_tree.hpp

include ../../templates/cpp-flags.make

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
