
## Requested input variables:
# TARGET_NAME
# SRC_ROOT

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
HOST := $(shell uname -s)

ifeq (${DEBUG}, 1)
	DEBUG_MODE := debug
else
	DEBUG_MODE := release
endif

ifeq (${HOST}, Darwin)
	DL_SUFFIX := dylib
else
	ifeq (${HOST}, Linux)
		DL_SUFFIX := so
	else
		DL_SUFFIX := dll
	endif
endif

# project things
OUT_DIR_NAME := $(shell echo ${HOST} | tr '[:upper:]' '[:lower:]')-${DEBUG_MODE}
OUT_DIR := $(CURDIR)/${OUT_DIR_NAME}
OBJ_ROOT := ${OUT_DIR}/obj/${TARGET_NAME}

# OBJS, DEPS
DIRS := $(shell find ${SRC_ROOT} -maxdepth 100 -type d)

C_SRCS := $(foreach dir,${DIRS},$(wildcard $(dir)/*.c))
C_OBJS := $(patsubst ${SRC_ROOT}%.c,${OBJ_ROOT}%.o,${C_SRCS})

CPP_SRCS := $(foreach dir,${DIRS},$(wildcard $(dir)/*.cpp))
CPP_OBJS := $(patsubst ${SRC_ROOT}%.cpp,${OBJ_ROOT}%.o,${CPP_SRCS})

OBJS := ${C_OBJS} ${CPP_OBJS}
DEPS := ${OBJS:.o=.d}

# mkdirs
$(shell mkdir -p $(patsubst ${SRC_ROOT}%,${OBJ_ROOT}%,${DIRS}))
