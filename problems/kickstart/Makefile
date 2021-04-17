.PHONY: new kickstart cpp build all debug clean run formatall

new kickstart cpp:
	@./kickstart.py cpp

debug:
	@+./all.sh BUILD debug

clean:
	@+./all.sh CLEAN clean

run:
	@+./all.sh RUN run

formatall:
	@clang-format --fallback-style=none -i 20*/**.cpp
