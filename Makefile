# * General Makefile

.PHONY: kickstart leet hashcode

kickstart:
	@TEMPLATE=template-kickstart-cpp ./scripts/make-kickstart.sh

leet:
	@TEMPLATE=template-leet-cpp ./scripts/make-leet.sh

hashcode:
	@TEMPLATE=template-hashcode-cpp ./scripts/make-hashcode.sh

example:
	@TEMPLATE=template-hashcode-cpp ./scripts/make-example.sh
