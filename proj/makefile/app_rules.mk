
# LD_FLAGS
ifeq (${HOST},Darwin)
	# 指定 rpath
	LD_FLAGS += -Wl,-rpath,@executable_path
	LD_FLAGS += -Wl,-rpath,@executable_path/../Frameworks
endif

## phony targets
.PHONY: run gdb cgdb nemiver valgrind

run: ${TARGET}
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${OUT_DIR} ;\
	(cd ${OUT_DIR} ; ./${TARGET_NAME})

gdb: ${TARGET}
	(cd ${OUT_DIR} ; gdb ./${TARGET_NAME})

cgdb: ${TARGET}
	(cd ${OUT_DIR} ; cgdb ./${TARGET_NAME})

lldb: ${TARGET}
	(cd ${OUT_DIR} ; lldb ./${TARGET_NAME})

nemiver: ${TARGET}
	(cd ${OUT_DIR} ; nemiver ./${TARGET_NAME})

valgrind: ${TARGET}
	(cd ${OUT_DIR} ; valgrind -v --leak-check=full ./${TARGET_NAME})

# NOTE: in linux, ${LIB} should be the last parameter of ${CXX}
${TARGET}: ${OBJS} ${LIB_DEPS}
	$(CXX) ${OBJS} ${LIB} ${LD_FLAGS} -o $@
