# * General Makefile

.PHONY: kickstart-cpp kickstart-java kickstart-python

kickstart-cpp:
	TEMPLATE=template-kickstart-cpp ./scripts/make-kickstart.sh

kickstart-java:
	TEMPLATE=template-kickstart-java ./scripts/make-kickstart.sh

kickstart-python:
	TEMPLATE=template-kickstart-python ./scripts/make-kickstart.sh

leet-cpp:
	TEMPLATE=template-leet-cpp ./scripts/make-leet.sh

leet-java:
	TEMPLATE=template-leet-java ./scripts/make-leet.sh

leet-python:
	TEMPLATE=template-leet-python ./scripts/make-leet.sh
