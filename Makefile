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

kickstart-interactive kickstart-interactive-cpp:
	@./scripts/kickstart.py cpp-interactive

codejam-cpp:
	@./scripts/codejam.py cpp

codejam-interactive codejam-interactive-cpp:
	@./scripts/codejam.py cpp-interactive

leet-cpp:
	@./scripts/leet.py cpp

leet-shell:
	@./scripts/leet.py shell

leet-js:
	@./scripts/leet.py js

formatall:
	@clang-format --fallback-style=none -i $(shell find . -name '*.cpp' -type f)
