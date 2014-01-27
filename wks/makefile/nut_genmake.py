#!/usr/bin/env python
#coding=utf-8

import sys,os

class Cfg:
    CC = 'g++' # 编译器
    LD = 'gcc' # 动态连接工具
    AR = 'ar' # 静态连接工具
    CC_FLAGS = '-Wall -g -rdynamic' # 编译选项

    SRC_DIRS = ['../../test'] # 源码路径
    INCLUDE_PATHS = ['../../src'] # INCLUDE 路径
    DEFINES = []

    OBJ_DIR = './output' # obj 输出路径
    LIBS = ['pthread'] # 库(自动添加lib前缀和.so后缀)
    LIB_DIRS = [OBJ_DIR] # .so、.a 等库文件路径
    TARGET = os.path.join(OBJ_DIR, 'test') # 目标文件路径

    TARGET_RUN = TARGET
    TARGET_RUN_DIR = "../MSVC/MSVC/" # 目标文件运行目录
    TARGET_RUN_ARGS = '' # 目标文件运行参数

if __name__ == '__main__':
    output = './makefile'
    if len(sys.argv) > 1:
        output = sys.argv[1]

    import cgenmake
    lines = cgenmake.genmake(Cfg)
    #print lines
    with open(output, 'w') as f:
        f.write(lines)
