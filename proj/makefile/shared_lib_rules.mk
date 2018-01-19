
## 生成动态链接库规则

# FLAGS
CFLAGS += -fPIC
CXXFLAGS += -fPIC

# LDFLAGS
ifeq (${HOST}, Darwin)
	# 相当于执行 `install_name_tool -id @rpath/lib${TARGET_NAME}.dylib ${TARGET}`
	LDFLAGS += -install_name @rpath/lib${TARGET_NAME}.dylib
endif

# TARGET
${TARGET}: ${OBJS} ${LIB_DEPS}
	${CXX} ${OBJS} ${LDFLAGS} -shared -o $@
