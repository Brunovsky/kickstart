.PHONY: all formatall clean run

default-target: all

formatall:
	@clang-format --fallback-style=none -i $(shell find . -name '*.cpp' -type f)

clean:
	@+./all.sh CLEAN clean

all:
	@+./all.sh BUILD

run:
	@+./all.sh RUN run
