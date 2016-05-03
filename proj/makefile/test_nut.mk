#!/user/bin/env make

DEBUG ?= 0

CC = g++
LD = gcc
AR = ar

# variables
SRC_ROOT = ../../src/test_nut
ifeq (${DEBUG}, 1)
	OUT_DIR = $(CURDIR)/debug
else
	OUT_DIR = $(CURDIR)/release
endif
OBJ_ROOT = ${OUT_DIR}/obj/test_nut
MAKEFILE = test_nut.mk

# INC
INC += -I${SRC_ROOT}/..

# CC_FLAGS
HOST = $(shell uname -s)
CC_FLAGS += -Wall -std=c++11
ifeq (${DEBUG}, 1)
	CC_FLAGS += -DDEBUG -g
else
	CC_FLAGS += -DNDEBUG -O2
endif
ifeq (${HOST}, Darwin)
	CC_FLAGS += -stdlib=libc++
else
	CC_FLAGS += -rdynamic
endif

# LIB
LIB += -L${OUT_DIR} -lnut
LIB_DEPS += ${OUT_DIR}/libnut.a
ifeq (${HOST}, Darwin)
	LIB += -lc++
else
	LIB += -lpthread
endif

# OBJS, DEPS
DIRS = $(shell find ${SRC_ROOT} -maxdepth 10 -type d)
CPPS = $(foreach dir,${DIRS},$(wildcard $(dir)/*.cpp))
OBJS = $(patsubst ${SRC_ROOT}%.cpp,${OBJ_ROOT}%.o,${CPPS})
DEPS = ${OBJS:.o=.d}

# TARGET
TARGET = ${OUT_DIR}/test_nut

# mkdirs
$(shell mkdir -p $(patsubst ${SRC_ROOT}%,${OBJ_ROOT}%,${DIRS}))

.PHONY: all others clean rebuild run gdb cgdb nemiver valgrind

all: others ${TARGET}

others:
	make -f nut.mk

clean:
	rm -rf ${OBJS}
	rm -rf ${DEPS}
	rm -rf ${TARGET}

rebuild: clean all

run: ${TARGET}
	${TARGET}

gdb: ${TARGET}
	gdb ${TARGET}

cgdb: ${TARGET}
	cgdb ${TARGET}

lldb: ${TARGET}
	lldb ${TARGET}

nemiver: ${TARGET}
	nemiver ${TARGET}

valgrind: ${TARGET}
	valgrind -v --leak-check=full ${TARGET}

# NOTE: in linux, ${LIB} should be the last parameter
${TARGET}: ${OBJS} ${LIB_DEPS} ${MAKEFILE}
	${CC} -o $@ ${OBJS} ${LIB}

${OBJ_ROOT}/%.o: ${SRC_ROOT}/%.cpp ${MAKEFILE}
	${CC} ${INC} ${DEF} ${CC_FLAGS} -c $< -o $@

## 动态生成依赖关系
# %.d: %.cpp
${OBJ_ROOT}/%.d: ${SRC_ROOT}/%.cpp ${MAKEFILE}
	@rm -f $@
	@# 向 *.d.$ 中写入 "xx/xx/*.d xx/xx/*.o:\" 这样一个字符串
	@echo '$@ $@.o:\' | sed 's/[.]d[.]o/.o/g' > $@.$$
	@# 向 *.d.$$ 中写入用 gcc -MM 生成的初始依赖关系
	${CC} ${INC} ${DEF} ${CC_FLAGS} -MM $< > $@.$$.$$
	@# 将 *.d.$$ 中内容去除冒号前的内容，剩余内容写入 *.d.$ 中
	@sed 's/^.*[:]//g' < $@.$$.$$ >> $@.$$
	@# 空行
	@echo '' >> $@.$$
	@# 对 *.d.$$ 的内容依此处理：
	@#	sed 去除冒号前的内容
	@#	sed 去除续行符
	@#	fmt 每个连续单词作为单独一行
	@#	sed 去除行首空白
	@#	sed 行尾添加冒号
	@sed -e 's/.*://' -e 's/\\$$//' < $@.$$.$$ | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $@.$$
	@# 最后清理
	@rm -f $@.$$.$$
	@mv $@.$$ $@

# 引入动态依赖关系
#	起首的'-'符号表示忽略错误命令(这里忽略不存在的文件，不再打warning)
-include ${DEPS}
