EXE := node code.js
NAIVEEXE := node naive.js

run code fast:
	@$(EXE) < input.txt

answer:
	@$(EXE) < input.txt > answer.txt

test:
	@$(EXE) < input.txt > program.txt
	@cmp program.txt naive.txt && echo OK

naive:
	@$(NAIVEEXE) < input.txt > naive.txt
