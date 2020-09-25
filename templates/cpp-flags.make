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
	WARNS += -Wshadow-uncaptured-local
	WARNS += -Wformat-non-iso -Wformat-pedantic
	WARNS += -Wextra-semi-stmt -Wnewline-eof
	WARNS += -Widiomatic-parentheses -Wmissing-braces -Wredundant-parens
	WARNS += -Wreorder -Wsigned-enum-bitfield -Wmissing-field-initializers
	WARNS += -Wmethod-signatures -Wstrict-prototypes
	WARNS += -Wover-aligned -Wpacked
	WARNS += -Wreturn-std-move
	WARNS += -Wself-assign -Wself-move
	WARNS += -Wmissing-noreturn
	WARNS += -Wtautological-compare -Wtautological-constant-in-range-compare
	WARNS += -Wundef -Wundefined-func-template -Wundefined-internal-type
	WARNS += -Wunused-const-variable -Wunused-exception-parameter
	WARNS += -Wvla
	WARNS += -Wweak-template-vtables -Wweak-vtables
	WARNS += -Wzero-as-null-pointer-constant -Wzero-length-array

    # Good warnings but disabled
    # WARNS += -Wheader-hygiene

    # Broken in clang 10
    # -Wshadow -Wno-shadow-field-in-constructor
endif

# Not errors
WARNS += -Wno-unused-function
WARNS += -Wno-missing-declarations
WARNS += -Wno-unknown-pragmas

# Too many spurious warnings
WARNS += -Wno-null-dereference

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

# Sanitizers

# * Sanitizers
# ? https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html

# Address sanitizer, fast memory error detector. Incompatible with =thread
# ? https://github.com/google/sanitizers/wiki#addresssanitizer
# Memory leak detector. Incompatible with =thread
# ? https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer
LEAK_SANIT := -fsanitize=address -fsanitize=leak
LEAK_SANIT += -fsanitize=pointer-compare
LEAK_SANIT += -fsanitize=pointer-subtract

# Thread sanitizer, fast data race sanitizer. Incompatible with =address, =leak
# ? https://github.com/google/sanitizers/wiki#threadsanitizer
THREAD_SANIT := -fsanitize=thread

# Enable one:
SANIT := $(LEAK_SANIT)
#SANIT := $(THREAD_SANIT)

# Undefined Behaviour detector.
UB_SANIT := -fsanitize=pointer-overflow
UB_SANIT += -fsanitize=builtin
UB_SANIT += -fsanitize=undefined
UB_SANIT +=   -fsanitize=shift
UB_SANIT +=     -fsanitize=shift-base
UB_SANIT +=     -fsanitize=shift-exponent
UB_SANIT +=   -fsanitize=integer-divide-by-zero
UB_SANIT +=   -fsanitize=return
UB_SANIT +=   -fsanitize=signed-integer-overflow
UB_SANIT +=   -fsanitize=bounds
UB_SANIT +=   -fsanitize=float-divide-by-zero
UB_SANIT +=   -fsanitize=float-cast-overflow
UB_SANIT +=   -fsanitize=bool
UB_SANIT +=   -fsanitize=enum
UB_SANIT +=   -fsanitize=vptr

SANIT += $(UB_SANIT)

SANIT_LIST := $(subst -fsanitize=,,${SANIT})

CXXFLAGS := $(WARNS)
