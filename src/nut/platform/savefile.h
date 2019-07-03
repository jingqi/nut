
#ifndef ___HEADFILE_2CF707F0_54CC_4B40_AF2C_9760D1864CBC_
#define ___HEADFILE_2CF707F0_54CC_4B40_AF2C_9760D1864CBC_

#include <string>

#include "../nut_config.h"
#include "fd.h"


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

    fd_type _fd = NUT_INVALID_FD;
};

}

#endif
