@path=%path%;D:\data\app\win\develop\QtSDK\5.3\mingw482_32\bin;D:\data\app\win\develop\QtSDK\Tools\mingw482_32\bin
@cd build
qmake ..\pro\nut.pro -r -spec win32-g++
mingw32-make
@cd ..
