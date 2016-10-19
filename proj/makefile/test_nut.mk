#!/user/bin/env make

TARGET_NAME = test_nut
SRC_ROOT = ../../src/${TARGET_NAME}

# preface rules
include preface_rules.mk

# INC
INC += -I${SRC_ROOT}/..

# DEF
DEF +=

# CXX_FLAGS
CXX_FLAGS += -std=c++11

# LIB LIB_DEPS
ifeq (${HOST}, Darwin)
	LIB_NUT = ${OUT_DIR}/libnut.dylib
else
	LIB += -lpthread
	LIB_NUT = ${OUT_DIR}/libnut.so
endif
LIB += -L${OUT_DIR} -lnut
LIB_DEPS += ${LIB_NUT}

# LD_FLAGS
LD_FLAGS +=

# TARGET
TARGET = ${OUT_DIR}/${TARGET_NAME}

.PHONY: all clean rebuild

all: ${TARGET}

clean:
	make -f nut.mk clean
	rm -rf ${OBJS}
	rm -rf ${DEPS}
	rm -rf ${TARGET}

rebuild:
	# 顺序执行，不会并行
	make -f test_nut.mk clean
	make -f test_nut.mk all

${LIB_NUT}:
	make -f nut.mk

# rules
include common_rules.mk
include app_rules.mk
