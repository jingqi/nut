
TARGET = nut_test
TEMPLATE = app

include(../global.pri)

QT -= core gui
CONFIG += console
CONFIG -= app_bundle

DEFINES += 

NUT_ROOT = $$PWD/../../../../src
SRC_ROOT = $${NUT_ROOT}/test

INCLUDEPATH += \
    $${NUT_ROOT} \
    $${SRC_ROOT}

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

unix:!mac: LIBS += -lrt
LIBS += -lnut
