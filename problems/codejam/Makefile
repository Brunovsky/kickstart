.PHONY: build clean run formatall

build:
	@+./all.sh BUILD

clean:
	@+./all.sh CLEAN clean

run:
	@+./all.sh RUN run

formatall:
	@clang-format --fallback-style=none -i */*/*.cpp
