
TARGET = test_nut
TEMPLATE = app

QT -= core gui
CONFIG += console
CONFIG -= app_bundle

# C++11 支持
QMAKE_CXXFLAGS += -std=c++11
mac: QMAKE_CXXFLAGS += -stdlib=libc++

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
SOURCES += $$files($${SRC_ROOT}/*.c*, true)

# 连接库
mac {
    LIBS += -lc++
} else: unix {
    LIBS += -lrt
} else {
    LIBS += -lpthread
}
# nut
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../nut/release -lnut
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../nut/debug -lnut
else:unix: LIBS += -L$$OUT_PWD/../nut -lnut

# dylib 安装路径
mac:contains(TEMPLATE, app) {
    QMAKE_LFLAGS += -Wl,-rpath,@executable_path -Wl,-rpath,@executable_path/../Frameworks
    QMAKE_LFLAGS_RPATH =
}
