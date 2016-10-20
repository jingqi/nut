
## 预定义变量和规则

# built-in variables
DEBUG ?= 0

MAKE ?= make

CC ?= gcc
CXX ?= g++

AR ?= ar
LD ?= ld

BISON ?= bison
FLEX ?= flex
RAGEL ?= ragel

# predefined variables
HOST = $(shell uname -s)

# project things
ifeq (${DEBUG}, 1)
	OUT_DIR = $(CURDIR)/debug
else
	OUT_DIR = $(CURDIR)/release
endif
OBJ_ROOT = ${OUT_DIR}/obj/${TARGET_NAME}

# OBJS, DEPS
DIRS = $(shell find ${SRC_ROOT} -maxdepth 100 -type d)
CPPS = $(foreach dir,${DIRS},$(wildcard $(dir)/*.cpp))
OBJS = $(patsubst ${SRC_ROOT}%.cpp,${OBJ_ROOT}%.o,${CPPS})
DEPS = ${OBJS:.o=.d}

# mkdirs
$(shell mkdir -p $(patsubst ${SRC_ROOT}%,${OBJ_ROOT}%,${DIRS}))
