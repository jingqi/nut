#!/user/bin/env make

all:
	make -f nut_makefile
	make -f test_nut_makefile

clean:
	make -f nut_makefile clean
	make -f test_nut_makefile clean

rebuild: clean all

run: all
	make -f test_nut_makefile run

cgdb: all
	make -f test_nut_makefile cgdb

valgrind: all
	make -f test_nut_makefile valgrind
