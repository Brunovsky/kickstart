.PHONY: default-target
default-target: run

# * Files

EXE := node code.js
NAIVE_EXE := node naive.js
GENERATE_EXE := node generate.js

# ---

clean:
	@rm -f program.txt

# ---

run fast:
	@$(EXE) < input.txt | tee program.txt

naive:
	@$(NAIVE_EXE) < input.txt | tee correct.txt

generate:
	@$(GENERATE_EXE) > input.txt

# ---

.PHONY: diff linediff

# Compare program.txt with correct.txt byte-by-byte
diff:
	@python ../../../../common/casediff.py program.txt correct.txt

# Compare program.txt with correct.txt line-by-line and output differing lines
linediff:
	@diff -y --tabsize=4 --minimal program.txt correct.txt
