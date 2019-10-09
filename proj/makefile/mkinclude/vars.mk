
## 预定义变量

# Built-in variables
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

CPPFLAGS ?=
CFLAGS ?=
CXXFLAGS ?=
OBJS ?=
DEPS ?=
LIB_DEPS ?=
ARFLAGS ?=
LDFLAGS ?=

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
