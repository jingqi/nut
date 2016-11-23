
TARGET = nut
TEMPLATE = lib
VERSION = 1.0.0

QT -= core gui

# C++11 支持
QMAKE_CXXFLAGS += -std=c++11
mac: QMAKE_CXXFLAGS += -stdlib=libc++

# XXX 这里貌似是qmake的一个bug，不会主动添加 _DEBUG/NDEBUG 宏
CONFIG(debug, debug|release): DEFINES += _DEBUG
else: DEFINES += NDEBUG

DEFINES += BUILDING_NUT_DLL

# INCLUDE 路径
SRC_ROOT = $$PWD/../../../../src/nut
INCLUDEPATH += \
    $${SRC_ROOT}/..

# 头文件
HEADERS += $$files($${SRC_ROOT}/*.h*, true)

# 源代码
SOURCES += $$files($${SRC_ROOT}/*.c*, true)

# 连接库
mac {
    LIBS += -lc++
} else: unix {
    LIBS += -lrt
} else {
    LIBS += -lpthread
}

# dylib 安装路径
mac:contains(TEMPLATE, lib):!contains(CONFIG, staticlib) {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
}

