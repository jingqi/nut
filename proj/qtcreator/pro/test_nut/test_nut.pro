
TARGET = test_nut
TEMPLATE = app

QT -= core gui
CONFIG += console
CONFIG -= app_bundle

# 配置输出目录
DESTDIR = $$PWD/../..
mac {
    DESTDIR = $${DESTDIR}/mac
} else : unix {
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
SRC_ROOT = $$PWD/../../../../src/test_nut
INCLUDEPATH += \
    $${SRC_ROOT}/.. \
    $${SRC_ROOT}

# 源代码
SOURCES +=\
    $$files($${SRC_ROOT}/*.c*) \
    $$files($${SRC_ROOT}/container/*.c*) \
    $$files($${SRC_ROOT}/container/skiplist/*.c*) \
    $$files($${SRC_ROOT}/container/tree/*.c*) \
    $$files($${SRC_ROOT}/container/tree/rtree/*.c*) \
    $$files($${SRC_ROOT}/debugging/*.c*) \
    $$files($${SRC_ROOT}/rc/*.c*) \
    $$files($${SRC_ROOT}/logging/*.c*) \
    $$files($${SRC_ROOT}/mem/*.c*) \
    $$files($${SRC_ROOT}/numeric/*.c*) \
    $$files($${SRC_ROOT}/pconsole/*.c*) \
    $$files($${SRC_ROOT}/platform/*.c*) \
    $$files($${SRC_ROOT}/security/*.c*) \
    $$files($${SRC_ROOT}/security/digest/*.c*) \
    $$files($${SRC_ROOT}/security/encrypt/*.c*) \
    $$files($${SRC_ROOT}/threading/*.c*) \
    $$files($${SRC_ROOT}/threading/lockfree/*.c*) \
    $$files($${SRC_ROOT}/util/*.c*) \
    $$files($${SRC_ROOT}/util/string/*.c*) \
    $$files($${SRC_ROOT}/util/txtcfg/*.c*) \
    $$files($${SRC_ROOT}/util/txtcfg/xml/*.c*)

# 连接库
unix {
    !mac: LIBS += -lrt
} else {
    LIBS += -lpthread
}
LIBS += -L$${DESTDIR} -lnut
