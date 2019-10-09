#!/usr/bin/env make

.PHONY: all clean rebuild run valgrind

all:
	$(MAKE) -f nut.mk
	$(MAKE) -f test-nut.mk

clean:
	$(MAKE) -f nut.mk clean
	$(MAKE) -f test-nut.mk clean

rebuild:
	$(MAKE) clean
	$(MAKE) all

run:
	$(MAKE) -f test-nut.mk run

valgrind:
	$(MAKE) -f test-nut.mk valgrind
