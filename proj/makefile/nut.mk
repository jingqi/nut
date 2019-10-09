#!/usr/bin/env make

# Preface
include mkinclude/vars.mk
include mkinclude/funcs.mk

# Project vars
TARGET_NAME = nut
SRC_ROOT = ../../src/${TARGET_NAME}
OBJ_ROOT = ${OUT_DIR}/obj/${TARGET_NAME}
TARGET = ${OUT_DIR}/lib${TARGET_NAME}.${DL_SUFFIX}

# Make dirs
$(call make_image_dir_tree,${SRC_ROOT},${OBJ_ROOT})

# C/C++ standard
CFLAGS += -std=c11
CXXFLAGS += -std=c++11

# Defines and flags
CPPFLAGS += -DEXPORT_NUT_API

# Includes
CPPFLAGS += -I${SRC_ROOT}/..

# Files
SRCS = $(call files,${SRC_ROOT},*.c *.cpp)
OBJS = $(patsubst ${SRC_ROOT}%.cpp,${OBJ_ROOT}%.o,$(patsubst ${SRC_ROOT}%.c,${OBJ_ROOT}%.o,${SRCS}))
DEPS = $(patsubst ${SRC_ROOT}%.cpp,${OBJ_ROOT}%.d,$(patsubst ${SRC_ROOT}%.c,${OBJ_ROOT}%.d,${SRCS}))

# Other libraries
ifeq (${HOST}, Linux)
	LDFLAGS += -lpthread -ldl -latomic
endif

# Targets
.PHONY: all clean rebuild

all: ${TARGET}

clean:
	${RM} ${OBJS} ${DEPS} ${TARGET}

rebuild:
	# 顺序执行，不会并行
	${MAKE} -f nut.mk clean
	${MAKE} -f nut.mk all

# Rules
include mkinclude/common_rules.mk
include mkinclude/shared_lib_rules.mk
