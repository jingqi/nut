
# XXX 这里貌似是qmake的一个bug，不会主动添加 _DEBUG/NDEBUG 宏
CONFIG(debug, debug|release): DEFINES += _DEBUG
else: DEFINES += NDEBUG

# C++11 支持
QMAKE_CXXFLAGS += -std=c++11
mac: QMAKE_CXXFLAGS += -stdlib=libc++

# 其他变量
CONFIG(debug, debug|release): DEBUG_MODE = debug
else: DEBUG_MODE = release

win32: OUT_TAIL = /$${DEBUG_MODE}

mac: OS_NAME = darwin
else:win32: OS_NAME = win
else:unix: OS_NAME = unix

# 链接库
mac {
    LIBS += -lc++
} else: win32 {
    LIBS += -lpthread
} else {
    LIBS += -lrt
}

# dylib 安装路径
mac: contains(TEMPLATE, lib): !contains(CONFIG, staticlib) {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
}
mac: contains(TEMPLATE, app) {
    QMAKE_LFLAGS += -Wl,-rpath,@executable_path -Wl,-rpath,@executable_path/../Frameworks
    QMAKE_LFLAGS_RPATH =
}
