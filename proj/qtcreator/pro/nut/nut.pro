
TARGET = nut
TEMPLATE = lib

include(../global.pri)

QT -= core gui
CONFIG += staticlib

DEFINES += 

SRC_ROOT = $$PWD/../../../../src

INCLUDEPATH += \
    $${SRC_ROOT}

HEADERS +=\
    $$files($${SRC_ROOT}/nut/*.h*) \
    $$files($${SRC_ROOT}/nut/container/*.h*) \
    $$files($${SRC_ROOT}/nut/container/skiplist/*.h*) \
    $$files($${SRC_ROOT}/nut/container/tree/*.h*) \
    $$files($${SRC_ROOT}/nut/container/tree/rtree/*.h*) \
    $$files($${SRC_ROOT}/nut/debugging/*.h*) \
    $$files($${SRC_ROOT}/nut/rc/*.h*) \
    $$files($${SRC_ROOT}/nut/logging/*.h*) \
    $$files($${SRC_ROOT}/nut/mem/*.h*) \
    $$files($${SRC_ROOT}/nut/memtool/*.h*) \
    $$files($${SRC_ROOT}/nut/numeric/*.h*) \
    $$files($${SRC_ROOT}/nut/numeric/numeric_algo/*.h*) \
    $$files($${SRC_ROOT}/nut/pconsole/*.h*) \
    $$files($${SRC_ROOT}/nut/platform/*.h*) \
    $$files($${SRC_ROOT}/nut/security/*.h*) \
    $$files($${SRC_ROOT}/nut/security/digest/*.h*) \
    $$files($${SRC_ROOT}/nut/security/encrypt/*.h*) \
    $$files($${SRC_ROOT}/nut/threading/*.h*) \
    $$files($${SRC_ROOT}/nut/threading/lockfree/*.h*) \
    $$files($${SRC_ROOT}/nut/threading/sync/*.h*) \
    $$files($${SRC_ROOT}/nut/unittest/*.h*) \
    $$files($${SRC_ROOT}/nut/util/*.h*) \
    $$files($${SRC_ROOT}/nut/util/sqlite3helper/*.h*) \
    $$files($${SRC_ROOT}/nut/util/string/*.h*) \
    $$files($${SRC_ROOT}/nut/util/timer/*.h*) \
    $$files($${SRC_ROOT}/nut/util/txtcfg/*.h*) \
    $$files($${SRC_ROOT}/nut/util/txtcfg/xml/*.h*)

SOURCES +=\
    $$files($${SRC_ROOT}/nut/*.c*) \
    $$files($${SRC_ROOT}/nut/container/*.c*) \
    $$files($${SRC_ROOT}/nut/container/skiplist/*.c*) \
    $$files($${SRC_ROOT}/nut/container/tree/*.c*) \
    $$files($${SRC_ROOT}/nut/container/tree/rtree/*.c*) \
    $$files($${SRC_ROOT}/nut/debugging/*.c*) \
    $$files($${SRC_ROOT}/nut/rc/*.c*) \
    $$files($${SRC_ROOT}/nut/logging/*.c*) \
    $$files($${SRC_ROOT}/nut/mem/*.c*) \
    $$files($${SRC_ROOT}/nut/memtool/*.c*) \
    $$files($${SRC_ROOT}/nut/numeric/*.c*) \
    $$files($${SRC_ROOT}/nut/numeric/numeric_algo/*.c*) \
    $$files($${SRC_ROOT}/nut/pconsole/*.c*) \
    $$files($${SRC_ROOT}/nut/platform/*.c*) \
    $$files($${SRC_ROOT}/nut/security/*.c*) \
    $$files($${SRC_ROOT}/nut/security/digest/*.c*) \
    $$files($${SRC_ROOT}/nut/security/encrypt/*.c*) \
    $$files($${SRC_ROOT}/nut/threading/*.c*) \
    $$files($${SRC_ROOT}/nut/threading/lockfree/*.c*) \
    $$files($${SRC_ROOT}/nut/threading/sync/*.c*) \
    $$files($${SRC_ROOT}/nut/unittest/*.c*) \
    $$files($${SRC_ROOT}/nut/util/*.c*) \
    $$files($${SRC_ROOT}/nut/util/sqlite3helper/*.c*) \
    $$files($${SRC_ROOT}/nut/util/string/*.c*) \
    $$files($${SRC_ROOT}/nut/util/timer/*.c*) \
    $$files($${SRC_ROOT}/nut/util/txtcfg/*.c*) \
    $$files($${SRC_ROOT}/nut/util/txtcfg/xml/*.c*)
