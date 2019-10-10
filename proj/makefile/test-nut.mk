#!/usr/bin/env make

# Preface
include mkinclude/vars.mk
include mkinclude/funcs.mk

# Project vars
TARGET_NAME = test-nut
SRC_ROOT = ../../src/${TARGET_NAME}
OBJ_ROOT = ${OUT_DIR}/obj/${TARGET_NAME}
TARGET = ${OUT_DIR}/${TARGET_NAME}

# C/C++ standard
CFLAGS += -std=c11
CXXFLAGS += -std=c++11

# Defines and flags
CPPFLAGS +=

# Includes

# Files
OBJS = $(call map_files,${SRC_ROOT},c,${OBJ_ROOT},o) \
    $(call map_files,${SRC_ROOT},cpp,${OBJ_ROOT},o)
DEPS = ${OBJS:.o=.d}

# Library nut
CPPFLAGS += -I${SRC_ROOT}/..
LDFLAGS += -L${OUT_DIR} -lnut
LIB_DEPS += ${OUT_DIR}/libnut.${DL_SUFFIX}

# Other libraries
ifeq (${HOST}, Linux)
    LDFLAGS += -lpthread -latomic
endif

# Targets
.PHONY: all
all: ${TARGET}

.PHONY: clean
clean:
	$(RM) ${OBJS} ${DEPS} ${TARGET}

.PHONY: rebuild
rebuild:
	@# 顺序执行，不会并行
	$(MAKE) -f test-nut.mk clean
	$(MAKE) -f test-nut.mk all

# Rules
include mkinclude/common_rules.mk
include mkinclude/app_rules.mk
