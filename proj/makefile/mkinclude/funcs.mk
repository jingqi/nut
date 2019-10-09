
## 工具函数

# 列举路径下所有子目录路径
#   参数1: 根路径
#
#   例如:
#     DIRS = $(call subdirs,${SRC_ROOT})
subdirs = $(shell find $(1) -type d \
	-not -path "*/linux-debug" -not -path "*/linux-debug/*" \
	-not -path "*/linux-release" -not -path "*/linux-release/*" \
	-not -path "*/darwin-debug" -not -path "*/darwin-debug/*" \
	-not -path "*/darwin-release" -not -path "*/darwin-release/*" \
	-not -path "*/windows-debug" -not -path "*/windows-debug/*" \
	-not -path "*/windows-release" -not -path "*/windows-release/*")

# 递归遍历目录, 获得所有文件
#   参数1: 根路径
#   参数2: 文件名通配模式, 以空格分开
#
#   例如:
#     SRCS = $(call files,${SRC_ROOT},*.c *.cpp)
files = $(foreach ptn,$(2),$(foreach dir,$(call subdirs,$(1)),$(wildcard ${dir}/${ptn})))

# 镜像创建目录树结构
#   参数1: 源根路径
#   参数2: 目标根路径
#
#   例如:
#     $(call make_image_dir_tree,${SRC_ROOT},${OBJ_ROOT})
make_image_dir_tree = $(shell mkdir -p $(patsubst $(1)%,$(2)%,$(call subdirs,$(1))))

# 递归遍历目录, 获得所有源文件, 然后转换成目标目录的目标文件
#   参数1: 源根路径
#   参数2: 源码后缀名
#   参数3: 目标根路径
#   参数4: 目标文件后缀名
#
#   例如:
#     $(call image_file_tree,${SRC_ROOT},c,${OBJ_ROOT},o)
image_file_tree = $(patsubst $(1)%.${2},$(3)%.$(4),$(call files,$(1),*.$(2)))

# 非递归遍历目录, 获得所有源文件, 然后转换成目标目录的目标文件
#   参数1: 源根路径
#   参数2: 源码后缀名
#   参数3: 目标根路径
#   参数4: 目标文件后缀名
#
#   例如:
#     $(call image_files,${SRC_ROOT},c,${OBJ_ROOT},o)
image_files = $(patsubst $(1)%.${2},$(3)%.$(4),$(wildcard $(1)/*.$(2)))
