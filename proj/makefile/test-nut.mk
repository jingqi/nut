#!/usr/bin/env make

# Preface
include mkinclude/vars.mk
include mkinclude/funcs.mk

# Project vars
TARGET_NAME = test-nut
SRC_ROOT = ../../src/${TARGET_NAME}
OBJ_ROOT = ${OUT_DIR}/obj/${TARGET_NAME}
TARGET = ${OUT_DIR}/${TARGET_NAME}

# Make dirs
$(call make_image_dir_tree,${SRC_ROOT},${OBJ_ROOT})

# C/C++ standard
CFLAGS += -std=c11
CXXFLAGS += -std=c++11

# Defines and flags
CPPFLAGS +=

# Includes
CPPFLAGS += -I${SRC_ROOT}/..

# Files
SRCS = $(call files,${SRC_ROOT},*.c *.cpp)
OBJS = $(patsubst ${SRC_ROOT}%.cpp,${OBJ_ROOT}%.o,$(patsubst ${SRC_ROOT}%.c,${OBJ_ROOT}%.o,${SRCS}))
DEPS = $(patsubst ${SRC_ROOT}%.cpp,${OBJ_ROOT}%.d,$(patsubst ${SRC_ROOT}%.c,${OBJ_ROOT}%.d,${SRCS}))

# Library nut
LIB_NUT = ${OUT_DIR}/libnut.${DL_SUFFIX}
LDFLAGS += -L${OUT_DIR} -lnut
LIB_DEPS += ${LIB_NUT}

# Other libraries
ifeq (${HOST}, Linux)
	LDFLAGS += -lpthread -latomic
endif

# Targets
.PHONY: all clean rebuild

all: ${TARGET}

clean:
	${MAKE} -f nut.mk clean
	${RM} ${OBJS} ${DEPS} ${TARGET}

rebuild:
	# 顺序执行，不会并行
	${MAKE} -f test-nut.mk clean
	${MAKE} -f test-nut.mk all

${LIB_NUT}: FORCE
	${MAKE} -f nut.mk

# Rules
include mkinclude/common_rules.mk
include mkinclude/app_rules.mk
