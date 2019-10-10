
## 预定义变量

# 内建变量
DEBUG ?= 0

MAKE ?= make

CC ?= gcc
CXX ?= g++

AR ?= ar
LD ?= ld

BISON ?= bison
FLEX ?= flex
RAGEL ?= ragel

RM ?= rm -f

# 用户应该修改的变量
CPPFLAGS ?=
CFLAGS ?=
CXXFLAGS ?=
ARFLAGS ?=
LDFLAGS ?=

SRC_ROOT ?=
OBJ_ROOT ?=
OBJS ?=
DEPS ?=
LIB_DEPS ?=
TARGET ?=

# Predefined variables
HOST := $(shell uname -s)
ifeq (${HOST}, Linux)
    DL_SUFFIX := so
else ifeq (${HOST}, Darwin)
    DL_SUFFIX := dylib
else
    # Windows 下 uname -s 显示比较不规则
    HOST := Windows
    DL_SUFFIX := dll
endif

ifeq (${DEBUG}, 1)
    DEBUG_MODE := debug
else
    DEBUG_MODE := release
endif

OUT_DIR_NAME := $(shell echo ${HOST} | tr '[:upper:]' '[:lower:]')-${DEBUG_MODE}
OUT_DIR := ${CURDIR}/${OUT_DIR_NAME}
