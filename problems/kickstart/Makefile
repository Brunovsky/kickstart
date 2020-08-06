.PHONY: new kickstart build debug clean run formatall
.PHONY: cpp

new kickstart: cpp

cpp:
	@./kickstart.py cpp

build:
	@+./all.sh BUILD

debug:
	@+./all.sh BUILD debug

clean:
	@+./all.sh CLEAN clean

run:
	@+./all.sh RUN run

formatall:
	@clang-format --fallback-style=none -i 20*/**.cpp
