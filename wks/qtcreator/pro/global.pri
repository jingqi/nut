
# 这里貌似是qmake的一个bug，不会主动添加 _DEBUG/NDEBUG 宏
CONFIG(debug, debug|release) {
    DEFINES += _DEBUG
} else {
    DEFINES += NDEBUG
}

# 配置输出目录
OUTDIR = $$PWD/..
mac {
    OUTDIR = $${OUTDIR}/mac
} else : unix {
    OUTDIR = $${OUTDIR}/unix
} else {
    OUTDIR = $${OUTDIR}/win
}
OUTDIR = $${OUTDIR}-$${QMAKE_HOST.arch}
CONFIG(debug, debug|release) {
    OUTDIR = $${OUTDIR}-debug
} else {
    OUTDIR = $${OUTDIR}-release
}
DESTDIR = $${OUTDIR}
message("DESTDIR: "$${DESTDIR})

# 全局lib目录
LIBS += -L$${DESTDIR}
!unix {
    LIBS += libpthread
}

