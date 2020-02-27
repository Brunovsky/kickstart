# * Leet Problem Makefile

EXE := ./code.sh

.PHONY: default-target

default-target: $(EXE)

.PHONY: run test clean

# Compilation rule
$(EXE):
	true

run: test

test:
	@$(EXE)

clean:
	@rm -f core log stdout
