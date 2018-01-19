
## 生成静态链接库规则

# TARGET
${TARGET}: ${OBJS}
	${RM} $@
	${AR} cqs ${ARFLAGS} $@ ${OBJS}
