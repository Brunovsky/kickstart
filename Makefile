# * General Makefile

.PHONY: cpp java python

cpp:
	TEMPLATE=template-cpp ./scripts/make-problem.sh

java:
	TEMPLATE=template-java ./scripts/make-problem.sh

python:
	TEMPLATE=template-python ./scripts/make-problem.sh
