#!/user/bin/env make

TARGET_NAME = test-nut
SRC_ROOT = ../../src/${TARGET_NAME}

# Preface rules
include preface_rules.mk

# Includes
CPPFLAGS += -I${SRC_ROOT}/..

# Defines
CPPFLAGS +=

# C/C++ standard
CFLAGS += -std=c11
CXXFLAGS += -std=c++11

# Libraries
ifeq (${HOST}, Linux)
	LDFLAGS += -lpthread -latomic
endif
LIB_NUT = ${OUT_DIR}/libnut.${DL_SUFFIX}
LIB_DEPS += ${LIB_NUT}
LDFLAGS += -L${OUT_DIR} -lnut

# TARGET
TARGET = ${OUT_DIR}/${TARGET_NAME}

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
include common_rules.mk
include app_rules.mk
