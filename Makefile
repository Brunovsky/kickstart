# Generate a new problem folder under problems/ with 'make FORMAT'

.PHONY: kickstart kickstart-cpp
.PHONY: leet leet-cpp

kickstart: kickstart-cpp
leet: leet-cpp

kickstart-cpp:
	@./scripts/kickstart.py cpp

leet-cpp:
	@./scripts/leet.py cpp

leet-shell:
	@./scripts/leet.py shell

formatall:
	@clang-format --fallback-style=none -i $(shell find . -name '*.cpp' -type f)
