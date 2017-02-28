
#include <assert.h>
#include <fcntl.h> // for ::open()
#include <stdio.h> // for ::remove()
#include <sys/stat.h> // for ::lstat()

#include "platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#else
#   include <unistd.h> // for ::write(), ::close()
#endif

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
#if NUT_PLATFORM_OS_WINDOWS
    if (INVALID_HANDLE_VALUE != _handle)
        commit();
#else
    if (_fd >= 0)
        commit();
#endif
}

bool SaveFile::open()
{
#if NUT_PLATFORM_OS_WINDOWS
    assert(INVALID_HANDLE_VALUE != _handle);
#else
    assert(_fd < 0);
#endif

#if NUT_PLATFORM_OS_WINDOWS
    _handle = ::CreateFileA(_tmp_path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    return INVALID_HANDLE_VALUE != _handle;
#else
    // 要覆盖的文件不能是一个目录
    struct stat info;
    if (0 == ::lstat(_path.c_str(), &info) && S_ISDIR(info.st_mode))
        return false;

    _fd = ::open(_tmp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW, 0664);
    return _fd >= 0;
#endif

}

bool SaveFile::write(const void *buf, size_t len)
{
    assert(nullptr != buf);

#if NUT_PLATFORM_OS_WINDOWS
    assert(INVALID_HANDLE_VALUE != _handle);
    DWORD wrote = 0;
    return FALSE != ::WriteFile(_handle, buf, len, &wrote, nullptr);
#else
    assert(_fd >= 0);
    return ::write(_fd, buf, len) >= 0;
#endif
}

void SaveFile::cancel()
{
#if NUT_PLATFORM_OS_WINDOWS
    if (INVALID_HANDLE_VALUE != _handle)
    {
        ::CloseHandle(_handle);
        _handle = INVALID_HANDLE_VALUE;
        ::remove(_tmp_path.c_str());
    }
#else
    if (_fd >= 0)
    {
        ::close(_fd);
        _fd = -1;
        ::remove(_tmp_path.c_str());
    }
#endif
}

bool SaveFile::commit()
{
#if NUT_PLATFORM_OS_WINDOWS
    assert(INVALID_HANDLE_VALUE != _handle);
    ::FlushFileBuffers(_handle);
    ::CloseHandle(_handle);
    _handle = INVALID_HANDLE_VALUE;
    return 0 == ::rename(_tmp_path.c_str(), _path.c_str());
#else
    assert(_fd >= 0);
    // NOTE 确保在重命名文件之前数据全部写入磁盘
    //      参见 http://www.tuicool.com/articles/BjA7JjQ
    ::fsync(_fd);
    ::close(_fd);
    _fd = -1;
    // NOTE rename() 是一个原子操作，能够保证被移动的数据不会丢失
    return 0 == ::rename(_tmp_path.c_str(), _path.c_str());
#endif
}

}
