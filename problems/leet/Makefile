.PHONY: new leet build clean run formatall
.PHONY: cpp shell js

new leet: cpp

cpp:
	@./leet.py cpp

shell:
	@./leet.py shell

js:
	@./leet.py js

build:
	@+./all.sh BUILD

clean:
	@+./all.sh CLEAN clean

run:
	@+./all.sh RUN run

formatall:
	@clang-format --fallback-style=none -i */*.cpp
