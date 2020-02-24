# Generate a new problem folder under problems/ with 'make FORMAT'

.PHONY: kickstart kickstart-cpp
.PHONY: leet leet-cpp

kickstart: kickstart-cpp
leet: leet-cpp

kickstart-cpp:
	@./scripts/kickstart.py kickstart-cpp

leet-cpp:
	@./scripts/leet.py leet-cpp

formatall:
	@clang-format --fallback-style=none -i $(shell find . -name '*.cpp' -type f)
