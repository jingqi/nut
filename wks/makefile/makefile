#!/user/bin/env make

CC = g++
LD = gcc
AR = ar

HOST = $(shell uname -s)
INC = -I../../src
ifeq ($(HOST), Darwin)
	LIB = -lstdc++
else
	LIB = -lpthread
endif
ifeq ($(HOST), Darwin)
	CC_FLAGS = -Wall -g
else
	CC_FLAGS = -Wall -g -rdynamic
endif

PWD = $(shell pwd)
DIRS = $(shell find ../../test -maxdepth 10 -type d)
CPPS = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJS = $(patsubst %.cpp,%.o,$(CPPS))
DEPS = $(patsubst %.cpp,%.d,$(CPPS))
TARGET = $(PWD)/test

all: $(TARGET)

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
	$(CC) $(LIB) $^ -o $@

%.o: %.cpp
	$(CC) $(INC) $(DEF) $(CC_FLAGS) -c $< -o $@

# 生成动态依赖关系
%.d: %.cpp
	rm -f $@ ;\
	$(CC) $(INC) $(DEF) -MM $< >$@.$$ ;\
	echo '$< $@:\\' | sed 's/[.]cpp/.o/g' >$@ ;\
	sed 's/^.*[:]//g' <$@.$$ >>$@ ;\
	rm -f $@.$$

# 引入动态依赖关系
include ${OBJS:.o=.d}
