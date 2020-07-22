# Compiler, gcc or clang
COMPILER := gcc
CPP_STANDARD := c++14

# Compiler command
CC := g++

CODE := code.cpp
NAIVE := naive.cpp
GEN := generate.cpp

TESTING_TOOL := python3 ./testing_tool.py
EXE := ./hack
NAIVEEXE := ./hack_naive
GENEXE := ./hack_generate

# * Dialect

CXX := $(CC) -std=$(CPP_STANDARD) -pipe -pthread -pedantic -march=native

# * Standard library

CLANG_LIBCPP := -stdlib=libc++
GLIBS_DEBUG := -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC

ifeq ($(COMPILER),clang)
#	CXX += $(CLANG_LIBCPP)
endif

# * Warnings

WARNS := -Wall -Wextra -Wpedantic

WARNS += -Wunused -Wno-unused-function
WARNS += -Wredundant-decls -Wmissing-declarations
WARNS += -Wuninitialized
WARNS += -Wextra-semi

WARNS += -Wcast-align -Wcast-qual -Wold-style-cast
WARNS += -Wsign-compare -Wfloat-equal -Wdouble-promotion

WARNS += -Wnull-dereference
WARNS += -Wformat
WARNS += -Woverloaded-virtual -Wnon-virtual-dtor
WARNS += -Wpessimizing-move -Wredundant-move

# GCC
ifeq ($(COMPILER),gcc)
	WARNS += -fmax-errors=5
	WARNS += -Wrestrict
	WARNS += -Wduplicated-cond -Wduplicated-branches
	WARNS += -Wuseless-cast
	WARNS += -Wlogical-op
	WARNS += -Wshadow=local
	WARNS += -Wformat-signedness -Wformat-truncation
	WARNS += -Wtrampolines
	WARNS += -Wsuggest-attribute=noreturn -Wsuggest-final-types
	WARNS += -Wsuggest-final-methods -Wsuggest-override
endif

# Clang
ifeq ($(COMPILER),clang)
	WARNS += -Qunused-arguments

	WARNS += -Wdeprecated -Wc++17-compat -Wgnu -Wgcc-compat
	WARNS += -Wunreachable-code -Wunreachable-code-aggressive
	WARNS += -Wloop-analysis -Wimplicit-fallthrough -Winfinite-recursion
	WARNS += -Wconditional-uninitialized -Wsometimes-uninitialized -Wstatic-self-init
	WARNS += -Watomic-implicit-seq-cst -Wthread-safety
	WARNS += -Wdate-time
	WARNS += -Wbad-function-cast
	WARNS += -Wfloat-conversion -Wstring-conversion
	WARNS += -Wchar-subscripts -Wshift-sign-overflow
	WARNS += -Wdynamic-exception-spec
	WARNS += -Wduplicate-enum -Wduplicate-method-arg -Wduplicate-method-match
	WARNS += -Wshadow -Wshadow-field -Wshadow-uncaptured-local
	WARNS += -Wformat-non-iso -Wformat-pedantic
	WARNS += -Wextra-semi-stmt -Wheader-hygiene -Wnewline-eof
	WARNS += -Widiomatic-parentheses -Wmissing-braces -Wredundant-parens
	WARNS += -Wreorder -Wsigned-enum-bitfield -Wmissing-field-initializers
	WARNS += -Wmethod-signatures -Wstrict-prototypes
	WARNS += -Wover-aligned -Wpacked
	WARNS += -Wreturn-std-move
	WARNS += -Wself-assign -Wself-move
	WARNS += -Wmissing-noreturn -Wmissing-prototypes -Wmissing-variable-declarations
	WARNS += -Wtautological-compare -Wtautological-constant-in-range-compare
	WARNS += -Wundef -Wundefined-func-template -Wundefined-internal-type
	WARNS += -Wunused-const-variable -Wunused-exception-parameter
	WARNS += -Wvla
	WARNS += -Wweak-template-vtables -Wweak-vtables
	WARNS += -Wzero-as-null-pointer-constant -Wzero-length-array
endif

# Not errors
WARNS += -Wno-unused-function
WARNS += -Wno-unused-variable
WARNS += -Wno-missing-declarations

# * Performance

OPTIM := -O3 -DNDEBUG -flto
OPTIM += -funroll-loops -finline-functions -ftree-vectorize

ifeq ($(COMPILER),gcc)
	OPTIM += -floop-nest-optimize
endif

# * Debug

DEBUG := -g -ggdb -gdwarf-4

ifeq ($(COMPILER),gcc)
	DEBUG += -fvar-tracking-assignments
	DEBUG += -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
endif

CXXFLAGS := $(WARNS)

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
	@$(TESTING_TOOL) $(EXE)

# Build in debug mode and run under valgrind
valgrind: debug
	@clear
	@$(TESTING_TOOL) valgrind $(EXE)

# Compile and run in perfm mode
fast: perfm
	@$(TESTING_TOOL) $(EXE)

# Compile and run the naive version in 'naive.cpp'
naive: build_naive
	@$(TESTING_TOOL) @$(NAIVEEXE)

# Compile and run the input generator in 'generate.cpp'
generate: build_generate
	@$(GENEXE) > input.txt

clean:
	@rm -f $(EXE) $(NAIVEEXE) $(GENEXE) core vgcore.* *.log
