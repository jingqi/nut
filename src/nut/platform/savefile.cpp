
#include <assert.h>
#include <fcntl.h> // for ::open()
#include <unistd.h> // for ::write(), ::close()
#include <stdio.h> // for ::remove()
#include <sys/stat.h> // for ::lstat()

#include "savefile.h"


namespace nut
{

SaveFile::SaveFile(const std::string& path)
    : _path(path)
{
    _tmp_path = path + ".tmp";
}

SaveFile::~SaveFile()
{
    if (_fd >= 0)
        commit();
}

bool SaveFile::open()
{
    assert(_fd < 0);

    // 要覆盖的文件不能是一个目录
    struct stat info;
    if (0 == ::lstat(_path.c_str(), &info) && S_ISDIR(info.st_mode))
        return false;

    _fd = ::open(_tmp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW, 0664);
    return _fd >= 0;
}

bool SaveFile::write(const void *buf, size_t len)
{
    assert(nullptr != buf && _fd >= 0);
    return ::write(_fd, buf, len) >= 0;
}

void SaveFile::cancel()
{
    if (_fd >= 0)
    {
        ::close(_fd);
        _fd = -1;
        ::remove(_tmp_path.c_str());
    }
}

bool SaveFile::commit()
{
    assert(_fd >= 0);

    // NOTE 确保在重命名文件之前数据全部写入磁盘
    //      参见 http://www.tuicool.com/articles/BjA7JjQ
    ::fsync(_fd);
    ::close(_fd);
    _fd = -1;
    // NOTE rename() 是一个原子操作，能够保证被移动的数据不会丢失
    return 0 == ::rename(_tmp_path.c_str(), _path.c_str());
}

}
