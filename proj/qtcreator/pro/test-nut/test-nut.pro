
TARGET = test-nut
TEMPLATE = app

include(../nut_common.pri)

QT -= qt
CONFIG += console
CONFIG -= app_bundle

# 源代码
SRC_ROOT = $$PWD/../../../../src/test-nut
           SOURCES += $$files($${SRC_ROOT}/*.c*, true)

# nut
INCLUDEPATH += $$PWD/../../../../src
LIBS += -L$$OUT_PWD/../nut$${OUT_TAIL}
win32: LIBS += -lnut1
else: LIBS += -lnut

# 其他链接库
win32: LIBS += -lDbghelp -latomic
