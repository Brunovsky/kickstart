# Generate a new problem folder under problems/ with 'make FORMAT'

.PHONY: formatall
.PHONY: kickstart kickstart-cpp
.PHONY: leet leet-cpp leet-shell
.PHONY: codejam codejam-cpp

kickstart: kickstart-cpp
leet: leet-cpp
codejam: codejam-cpp

kickstart-cpp:
	@./scripts/kickstart.py cpp

leet-cpp:
	@./scripts/leet.py cpp

leet-shell:
	@./scripts/leet.py shell

codejam-cpp:
	@./scripts/codejam.py cpp

formatall:
	@clang-format --fallback-style=none -i $(shell find . -name '*.cpp' -type f)
