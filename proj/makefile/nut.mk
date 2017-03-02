#!/user/bin/env make

TARGET_NAME = nut
SRC_ROOT = ../../src/${TARGET_NAME}

# preface rules
include preface_rules.mk

# INC
INC += -I${SRC_ROOT}/..

# DEF
DEF += -DBUILDING_NUT_DLL

# CXX_FLAGS
CXX_FLAGS += -std=c++11

# LIB LIB_DEPS
LIB_DEPS +=
ifeq (${HOST}, Linux)
	LIB += -lpthread -ldl
endif

# LD_FLAGS
LD_FLAGS +=

# TARGET
TARGET = ${OUT_DIR}/lib${TARGET_NAME}.${DL_SUFFIX}

.PHONY: all clean rebuild

all: ${TARGET}

clean:
	rm -rf ${OBJS}
	rm -rf ${DEPS}
	rm -rf ${TARGET}

rebuild:
	# 顺序执行，不会并行
	$(MAKE) -f nut.mk clean
	$(MAKE) -f nut.mk all

# rules
include common_rules.mk
include shared_lib_rules.mk
