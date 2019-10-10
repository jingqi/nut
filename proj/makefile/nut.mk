#!/usr/bin/env make

# Preface
include mkinclude/vars.mk
include mkinclude/funcs.mk

# Project vars
TARGET_NAME = nut
SRC_ROOT = ../../src/${TARGET_NAME}
OBJ_ROOT = ${OUT_DIR}/obj/${TARGET_NAME}
TARGET = ${OUT_DIR}/lib${TARGET_NAME}.${DL_SUFFIX}

# C/C++ standard
CFLAGS += -std=c11
CXXFLAGS += -std=c++11

# Defines and flags
CPPFLAGS += -DEXPORT_NUT_API

# Includes
CPPFLAGS += -I${SRC_ROOT}/..

# Files
OBJS = $(call map_files,${SRC_ROOT},c,${OBJ_ROOT},o) \
    $(call map_files,${SRC_ROOT},cpp,${OBJ_ROOT},o)
DEPS = ${OBJS:.o=.d}

# Other libraries
ifeq (${HOST}, Linux)
    LDFLAGS += -lpthread -ldl -latomic
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
	$(MAKE) -f nut.mk clean
	$(MAKE) -f nut.mk all

# Rules
include mkinclude/common_rules.mk
include mkinclude/shared_lib_rules.mk
