
TARGET = nut
TEMPLATE = lib

QT -= core gui
CONFIG += staticlib

# 配置输出目录
DESTDIR = $$PWD/../..
mac {
    DESTDIR = $${DESTDIR}/mac
} else: unix {
    DESTDIR = $${DESTDIR}/unix
} else {
    DESTDIR = $${DESTDIR}/win
}
DESTDIR = $${DESTDIR}-$${QMAKE_HOST.arch}
CONFIG(debug, debug|release) {
    DESTDIR = $${DESTDIR}-debug
} else {
    DESTDIR = $${DESTDIR}-release
}
message("DESTDIR: "$${DESTDIR})

# C++11 支持
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CFLAGS += -std=c++11

# 这里貌似是qmake的一个bug，不会主动添加 _DEBUG/NDEBUG 宏
CONFIG(debug, debug|release) {
    DEFINES += _DEBUG
} else {
    DEFINES += NDEBUG
}

# INCLUDE 路径
SRC_ROOT = $$PWD/../../../../src/nut
INCLUDEPATH += \
    $${SRC_ROOT}/..

# 头文件
HEADERS +=\
    $$files($${SRC_ROOT}/*.h*) \
    $$files($${SRC_ROOT}/container/*.h*) \
    $$files($${SRC_ROOT}/container/bytestream/*.h*) \
    $$files($${SRC_ROOT}/container/skiplist/*.h*) \
    $$files($${SRC_ROOT}/container/tree/*.h*) \
    $$files($${SRC_ROOT}/container/tree/rtree/*.h*) \
    $$files($${SRC_ROOT}/debugging/*.h*) \
    $$files($${SRC_ROOT}/rc/*.h*) \
    $$files($${SRC_ROOT}/logging/*.h*) \
    $$files($${SRC_ROOT}/mem/*.h*) \
    $$files($${SRC_ROOT}/memtool/*.h*) \
    $$files($${SRC_ROOT}/numeric/*.h*) \
    $$files($${SRC_ROOT}/numeric/numeric_algo/*.h*) \
    $$files($${SRC_ROOT}/pconsole/*.h*) \
    $$files($${SRC_ROOT}/platform/*.h*) \
    $$files($${SRC_ROOT}/security/*.h*) \
    $$files($${SRC_ROOT}/security/digest/*.h*) \
    $$files($${SRC_ROOT}/security/encrypt/*.h*) \
    $$files($${SRC_ROOT}/threading/*.h*) \
    $$files($${SRC_ROOT}/threading/lockfree/*.h*) \
    $$files($${SRC_ROOT}/threading/sync/*.h*) \
    $$files($${SRC_ROOT}/unittest/*.h*) \
    $$files($${SRC_ROOT}/util/*.h*) \
    $$files($${SRC_ROOT}/util/sqlite3helper/*.h*) \
    $$files($${SRC_ROOT}/util/string/*.h*) \
    $$files($${SRC_ROOT}/util/time/*.h*) \
    $$files($${SRC_ROOT}/util/txtcfg/*.h*) \
    $$files($${SRC_ROOT}/util/txtcfg/xml/*.h*)

# 源代码
SOURCES +=\
    $$files($${SRC_ROOT}/*.c*) \
    $$files($${SRC_ROOT}/container/*.c*) \
    $$files($${SRC_ROOT}/container/bytestream/*.c*) \
    $$files($${SRC_ROOT}/container/skiplist/*.c*) \
    $$files($${SRC_ROOT}/container/tree/*.c*) \
    $$files($${SRC_ROOT}/container/tree/rtree/*.c*) \
    $$files($${SRC_ROOT}/debugging/*.c*) \
    $$files($${SRC_ROOT}/rc/*.c*) \
    $$files($${SRC_ROOT}/logging/*.c*) \
    $$files($${SRC_ROOT}/mem/*.c*) \
    $$files($${SRC_ROOT}/memtool/*.c*) \
    $$files($${SRC_ROOT}/numeric/*.c*) \
    $$files($${SRC_ROOT}/numeric/numeric_algo/*.c*) \
    $$files($${SRC_ROOT}/pconsole/*.c*) \
    $$files($${SRC_ROOT}/platform/*.c*) \
    $$files($${SRC_ROOT}/security/*.c*) \
    $$files($${SRC_ROOT}/security/digest/*.c*) \
    $$files($${SRC_ROOT}/security/encrypt/*.c*) \
    $$files($${SRC_ROOT}/threading/*.c*) \
    $$files($${SRC_ROOT}/threading/lockfree/*.c*) \
    $$files($${SRC_ROOT}/threading/sync/*.c*) \
    $$files($${SRC_ROOT}/unittest/*.c*) \
    $$files($${SRC_ROOT}/util/*.c*) \
    $$files($${SRC_ROOT}/util/sqlite3helper/*.c*) \
    $$files($${SRC_ROOT}/util/string/*.c*) \
    $$files($${SRC_ROOT}/util/time/*.c*) \
    $$files($${SRC_ROOT}/util/txtcfg/*.c*) \
    $$files($${SRC_ROOT}/util/txtcfg/xml/*.c*)

# 连接库
!unix {
    LIBS += -lpthread
}
