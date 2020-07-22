.PHONY: new kickstart build clean run formatall
.PHONY: cpp cpp-interactive

new kickstart: cpp

cpp:
	@./kickstart.py cpp

cpp-interactive:
	@./kickstart.py cpp-interactive

build:
	@+./all.sh BUILD

clean:
	@+./all.sh CLEAN clean

run:
	@+./all.sh RUN run

formatall:
	@clang-format --fallback-style=none -i 20*/**.cpp
