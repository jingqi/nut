#!/user/bin/env make

all:
	make -f test_nut.mk

clean:
	make -f nut.mk clean
	make -f test_nut.mk clean

rebuild: clean all

run: all
	make -f test_nut.mk run

cgdb: all
	make -f test_nut.mk cgdb

valgrind: all
	make -f test_nut.mk valgrind
