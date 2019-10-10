#!/usr/bin/env make

# Preface
include mkinclude/vars.mk

# Targets
.PHONY: all
all: nut test-nut

.PHONY: nut
nut:
	$(MAKE) -f nut.mk

.PHONY: test-nut
test-nut: nut
	$(MAKE) -f test-nut.mk

.PHONY: clean
clean:
	$(RM) -r ${OUT_DIR_NAME}

.PHONY: rebuild
rebuild:
	$(MAKE) clean
	$(MAKE) all

.PHONY: run
run: test-nut
	$(MAKE) -f test-nut.mk run

.PHONY: valgrind
valgrind: test-nut
	$(MAKE) -f test-nut.mk valgrind
