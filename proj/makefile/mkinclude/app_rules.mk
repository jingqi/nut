
## 生成可执行文件规则

# LDFLAGS
ifeq (${HOST},Darwin)
    # 指定 rpath
    LDFLAGS += -Wl,-rpath,@executable_path
    LDFLAGS += -Wl,-rpath,@executable_path/../Frameworks
endif

## Phony targets
.PHONY: run
run: ${TARGET}
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${OUT_DIR} ;\
	(cd ${OUT_DIR} ; ./${TARGET_NAME})

.PHONY: gdb
gdb: ${TARGET}
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${OUT_DIR} ;\
	(cd ${OUT_DIR} ; gdb ./${TARGET_NAME})

.PHONY: cgdb
cgdb: ${TARGET}
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${OUT_DIR} ;\
	(cd ${OUT_DIR} ; cgdb ./${TARGET_NAME})

.PHONY: lldb
lldb: ${TARGET}
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${OUT_DIR} ;\
	(cd ${OUT_DIR} ; lldb ./${TARGET_NAME})

.PHONY: nemiver
nemiver: ${TARGET}
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${OUT_DIR} ;\
	(cd ${OUT_DIR} ; nemiver ./${TARGET_NAME})

.PHONY: valgrind
valgrind: ${TARGET}
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${OUT_DIR} ;\
	(cd ${OUT_DIR} ; valgrind -v --leak-check=full ./${TARGET_NAME})

# NOTE In linux, '-l' should come behind '.o' files as command line argument \
#      of ${CXX}
${TARGET}: ${OBJS} ${LIB_DEPS}
	${CXX} ${OBJS} ${LDFLAGS} -o $@
