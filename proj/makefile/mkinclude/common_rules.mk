
## 通用规则

# CFLAGS, CXXFLAGS
CFLAGS += -Wall
CXXFLAGS += -Wall
ifeq (${DEBUG}, 1)
	CPPFLAGS += -DDEBUG
	CFLAGS += -g
	CXXFLAGS += -g
else
	CPPFLAGS += -DNDEBUG
	CFLAGS += -O2
	CXXFLAGS += -O2
endif
ifeq (${HOST}, Darwin)
	CXXFLAGS += -stdlib=libc++
else
	ifeq (${HOST}, Linux)
		CFLAGS += -rdynamic
		CXXFLAGS += -rdynamic
	endif
endif

# C++ standard library
ifeq (${HOST}, Darwin)
	LDFLAGS += -lc++
else
	LDFLAGS += -lstdc++
endif

# Phony target used to mark force executing
.PHONY: FORCE

## *.c -> *.o
## *.cpp -> *.o
define make-c-obj =
	${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o $@
endef

define make-cxx-obj =
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -c $< -o $@
endef

${OBJ_ROOT}/%.o: ${SRC_ROOT}/%.c
	${make-c-obj}

${OBJ_ROOT}/%.o: ${SRC_ROOT}/%.cpp
	${make-cxx-obj}

## Automatically make dependence rules
define make-c-dep =
	@${RM} $@
	@# 向 *.d.$ 中写入 "xx/xx/*.d xx/xx/*.o:\" 这样一个字符串
	@echo '$@ $@.o:\' | sed 's/[.]d[.]o/.o/g' > $@.$$
	@# 向 *.d.$.$ 中写入用 gcc -MM 生成的初始依赖关系
	${CC} ${CPPFLAGS} ${CFLAGS} -MM $< > $@.$$.$$
	@# 将 *.d.$.$ 中内容去除冒号前的内容，剩余内容写入 *.d.$ 中
	@sed 's/^.*[:]//g' < $@.$$.$$ >> $@.$$
	@# 空行
	@echo '' >> $@.$$
	@# 对 *.d.$.$ 的内容依此处理：
	@#	sed 去除冒号前的内容
	@#	sed 去除续行符
	@#	fmt 每个连续单词作为单独一行
	@#	sed 去除行首空白
	@#	sed 行尾添加冒号
	@sed -e 's/.*://' -e 's/\\$$//' < $@.$$.$$ | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $@.$$
	@# 最后清理
	@${RM} $@.$$.$$
	@mv $@.$$ $@
endef

define make-cxx-dep =
	@${RM} $@
	@# 向 *.d.$ 中写入 "xx/xx/*.d xx/xx/*.o:\" 这样一个字符串
	@echo '$@ $@.o:\' | sed 's/[.]d[.]o/.o/g' > $@.$$
	@# 向 *.d.$.$ 中写入用 gcc -MM 生成的初始依赖关系
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -MM $< > $@.$$.$$
	@# 将 *.d.$.$ 中内容去除冒号前的内容，剩余内容写入 *.d.$ 中
	@sed 's/^.*[:]//g' < $@.$$.$$ >> $@.$$
	@# 空行
	@echo '' >> $@.$$
	@# 对 *.d.$.$ 的内容依此处理：
	@#	sed 去除冒号前的内容
	@#	sed 去除续行符
	@#	fmt 每个连续单词作为单独一行
	@#	sed 去除行首空白
	@#	sed 行尾添加冒号
	@sed -e 's/.*://' -e 's/\\$$//' < $@.$$.$$ | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $@.$$
	@# 最后清理
	@${RM} $@.$$.$$
	@mv $@.$$ $@
endef

# %.d %.o: %.c
${OBJ_ROOT}/%.d: ${SRC_ROOT}/%.c
	${make-c-dep}

# %.d %.o: %.cpp
${OBJ_ROOT}/%.d: ${SRC_ROOT}/%.cpp
	${make-cxx-dep}

# 引入动态依赖关系
#	起首的'-'符号表示忽略错误命令(这里忽略不存在的文件，不再打warning)
-include ${DEPS}
