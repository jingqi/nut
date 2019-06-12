
#ifndef ___HEADFILE_2CF707F0_54CC_4B40_AF2C_9760D1864CBC_
#define ___HEADFILE_2CF707F0_54CC_4B40_AF2C_9760D1864CBC_

#include <string>

#include "../nut_config.h"
#include "platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif


namespace nut
{

/**
 * 安全的保存文件，避免崩溃时丢失原始文件数据
 */
class NUT_API SaveFile
{
public:
    explicit SaveFile(const std::string& path) noexcept;
    ~SaveFile() noexcept;

    bool open() noexcept;
    bool write(const void *buf, size_t len) noexcept;
    void cancel() noexcept;
    bool commit() noexcept;

private:
    std::string _path, _tmp_path;

#if NUT_PLATFORM_OS_WINDOWS
    // INVALID_HANDLE_VALUE is an invalid value returned by ::CreateFile()
    HANDLE _handle = INVALID_HANDLE_VALUE;
#else
    // -1 is an invalid value returned by ::open()
    int _fd = -1;
#endif
};

}

#endif
