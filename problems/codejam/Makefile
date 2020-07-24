.PHONY: new codejam build debug clean run formatall
.PHONY: cpp cpp-interactive

new codejam: cpp

cpp:
	@./codejam.py cpp

cpp-interactive:
	@./codejam.py cpp-interactive

build:
	@+./all.sh BUILD

debug:
	@+./all.sh BUILD debug

clean:
	@+./all.sh CLEAN clean

run:
	@+./all.sh RUN run

formatall:
	@clang-format --fallback-style=none -i */*/*.cpp
