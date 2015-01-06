
TARGET = nut_test
TEMPLATE = app

include(global.pri)

QT -= core gui

DEFINES += 

NUT_ROOT = $$PWD/../../../src
SRC_ROOT = $$PWD/../../../test

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
    $$files($${SRC_ROOT}/gc/*.c*) \
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

HEADERS +=\
        $$files($${NUT_ROOT}/nut/*.h*) \
        $$files($${NUT_ROOT}/nut/container/*.h*) \
        $$files($${NUT_ROOT}/nut/container/skiplist/*.h*) \
        $$files($${NUT_ROOT}/nut/container/tree/*.h*) \
        $$files($${NUT_ROOT}/nut/container/tree/rtree/*.h*) \
        $$files($${NUT_ROOT}/nut/debugging/*.h*) \
        $$files($${NUT_ROOT}/nut/gc/*.h*) \
        $$files($${NUT_ROOT}/nut/logging/*.h*) \
        $$files($${NUT_ROOT}/nut/mem/*.h*) \
        $$files($${NUT_ROOT}/nut/memtool/*.h*) \
        $$files($${NUT_ROOT}/nut/numeric/*.h*) \
        $$files($${NUT_ROOT}/nut/numeric/numeric_algo/*.h*) \
        $$files($${NUT_ROOT}/nut/pconsole/*.h*) \
        $$files($${NUT_ROOT}/nut/platform/*.h*) \
        $$files($${NUT_ROOT}/nut/security/*.h*) \
        $$files($${NUT_ROOT}/nut/security/digest/*.h*) \
        $$files($${NUT_ROOT}/nut/security/encrypt/*.h*) \
        $$files($${NUT_ROOT}/nut/threading/*.h*) \
        $$files($${NUT_ROOT}/nut/threading/lockfree/*.h*) \
        $$files($${NUT_ROOT}/nut/threading/sync/*.h*) \
        $$files($${NUT_ROOT}/nut/unittest/*.h*) \
        $$files($${NUT_ROOT}/nut/util/*.h*) \
        $$files($${NUT_ROOT}/nut/util/sqlite3helper/*.h*) \
        $$files($${NUT_ROOT}/nut/util/string/*.h*) \
        $$files($${NUT_ROOT}/nut/util/timer/*.h*) \
        $$files($${NUT_ROOT}/nut/util/txtcfg/*.h*) \
        $$files($${NUT_ROOT}/nut/util/txtcfg/xml/*.h*)
