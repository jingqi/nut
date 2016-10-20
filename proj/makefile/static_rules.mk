
## 生成静态链接库规则

# TARGET
${TARGET}: ${OBJS}
	rm -f $@
	$(AR) cqs $@ ${OBJS}
