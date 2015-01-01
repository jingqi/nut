#!/user/bin/env make

CC=g++
LD=gcc
AR=ar

INC=-I../../src
LIB=-lpthread
CC_FLAGS=$(INC) $(LIB) -Wall -g -rdynamic

PWD=$(shell pwd)
DIRS=$(shell find ../../test -maxdepth 10 -type d)
CPPS=$(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJS=$(patsubst %.cpp,%.o,$(CPPS))
DEPS=$(patsubst %.cpp,%.d,$(CPPS))
TARGET=$(PWD)/test

all: $(TARGET)

help:
	@echo
	@echo targets: all clean rebuild run gdb cgdb nemiver valgrind
	@echo ${OBJS:.o=.d}
	@echo

clean:
	rm -rf $(OBJS)
	rm -rf $(DEPS)
	rm -rf $(TARGET)

rebuild: clean all

run: $(TARGET)
	$(TARGET)

gdb: $(TARGET)
	gdb $(TARGET)

cgdb: $(TARGET)
	cgdb $(TARGET)

nemiver: $(TARGET)
	nemiver $(TARGET)

valgrind: $(TARGET)
	valgrind -v --leak-check=full $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CC_FLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CC_FLAGS) -c $< -o $@

# 生成动态依赖关系
%.d: %.cpp
	rm -f $@
	$(CC) $(CC_FLAGS) -MM $< >$@.$$
	echo '$< $@:\\\c' | sed 's/cpp/o/g' >$@
	sed 's/^.*[:]//g' <$@.$$ >>$@
	rm -f $@.$$

# 引入动态依赖关系
include ${OBJS:.o=.d}
