
## 生成动态链接库规则

# CXX_FLAGS
CXX_FLAGS += -fPIC

# LD_FLAGS
ifeq (${HOST}, Darwin)
	# 相当于执行 `install_name_tool -id @rpath/lib${TARGET_NAME}.dylib ${TARGET}`
	LD_FLAGS += -install_name @rpath/lib${TARGET_NAME}.dylib
endif

# TARGET
${TARGET}: ${OBJS} ${LIB_DEPS}
	$(CXX) ${OBJS} ${LIB} ${LD_FLAGS} -shared -o $@
