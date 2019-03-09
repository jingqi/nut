
TARGET = nut
TEMPLATE = lib
VERSION = 1.1.0

include(../nut_common.pri)

QT -= qt

DEFINES += BUILDING_NUT

# INCLUDE 路径
SRC_ROOT = $$PWD/../../../../src/nut
INCLUDEPATH += $${SRC_ROOT}/..

# 头文件
HEADERS += $$files($${SRC_ROOT}/*.h*, true)

# 源代码
SOURCES += $$files($${SRC_ROOT}/*.c*, true)
