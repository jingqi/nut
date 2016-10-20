
## 通用规则

# CXX_FLAGS
CXX_FLAGS += -Wall
ifeq (${DEBUG}, 1)
	CXX_FLAGS += -DDEBUG -g
else
	CXX_FLAGS += -DNDEBUG -O2
endif
ifeq (${HOST}, Darwin)
	CXX_FLAGS += -stdlib=libc++
else
	ifeq (${HOST}, Linux)
		CXX_FLAGS += -rdynamic
	endif
endif

# LIB
ifeq (${HOST}, Darwin)
	LIB += -lc++
else
	LIB += -lstdc++
endif

# phony target used to mark force executing
.PHONY: FORCE

## *.cpp -> *.o
${OBJ_ROOT}/%.o: ${SRC_ROOT}/%.cpp
	$(CXX) ${INC} ${DEF} ${CXX_FLAGS} -c $< -o $@

## automatically make dependence rules
# %.d %.o: %.cpp
${OBJ_ROOT}/%.d: ${SRC_ROOT}/%.cpp
	@rm -f $@
	@# 向 *.d.$ 中写入 "xx/xx/*.d xx/xx/*.o:\" 这样一个字符串
	@echo '$@ $@.o:\' | sed 's/[.]d[.]o/.o/g' > $@.$$
	@# 向 *.d.$.$ 中写入用 gcc -MM 生成的初始依赖关系
	$(CXX) ${INC} ${DEF} ${CXX_FLAGS} -MM $< > $@.$$.$$
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
	@rm -f $@.$$.$$
	@mv $@.$$ $@

# 引入动态依赖关系
#	起首的'-'符号表示忽略错误命令(这里忽略不存在的文件，不再打warning)
-include ${DEPS}
