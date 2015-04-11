
all:
	make -f nut_makefile
	make -f test_makefile

clean:
	make -f nut_makefile clean
	make -f test_makefile clean

rebuild: clean all

run: all
	make -f test_makefile run

cgdb: all
	make -f test_makefile cgdb

valgrind: all
	make -f test_makefile valgrind
