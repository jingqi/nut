#!/user/bin/env make

TARGET_NAME = nut
SRC_ROOT = ../../src/${TARGET_NAME}

# preface rules
include preface_rules.mk

# Includes
CPPFLAGS += -I${SRC_ROOT}/..

# Defines
CPPFLAGS += -DBUILDING_NUT_DLL

# C++ standard
CXXFLAGS += -std=c++11

# Libraries
LIB_DEPS +=
ifeq (${HOST}, Linux)
	LDFLAGS += -lpthread -ldl
endif

# TARGET
TARGET = ${OUT_DIR}/lib${TARGET_NAME}.${DL_SUFFIX}

.PHONY: all clean rebuild

all: ${TARGET}

clean:
	${RM} ${OBJS} ${DEPS} ${TARGET}

rebuild:
	# 顺序执行，不会并行
	${MAKE} -f nut.mk clean
	${MAKE} -f nut.mk all

# rules
include common_rules.mk
include shared_lib_rules.mk
