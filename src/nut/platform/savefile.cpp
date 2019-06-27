
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
#include "path.h"
#include "os.h"


namespace nut
{

SaveFile::SaveFile(const std::string& path) noexcept
    : _path(Path::realpath(path))
{
    _tmp_path = _path + ".tmp";
}

SaveFile::~SaveFile() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    if (INVALID_HANDLE_VALUE != _handle)
        commit();
#else
    if (_fd >= 0)
        commit();
#endif
}

bool SaveFile::open() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    assert(INVALID_HANDLE_VALUE == _handle);
    _handle = ::CreateFileA(_tmp_path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    return INVALID_HANDLE_VALUE != _handle;
#else
    assert(_fd < 0);

    // 要覆盖的文件不能是一个目录
    struct stat info;
    if (0 == ::lstat(_path.c_str(), &info) && !S_ISREG(info.st_mode))
        return false;

    _fd = ::open(_tmp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW, 0664);
    return _fd >= 0;
#endif
}

bool SaveFile::write(const void *buf, size_t len) noexcept
{
    assert(nullptr != buf || 0 == len);
    if (nullptr == buf || 0 == len)
        return true;

#if NUT_PLATFORM_OS_WINDOWS
    assert(INVALID_HANDLE_VALUE != _handle);
    DWORD wrote = 0;
    return FALSE != ::WriteFile(_handle, buf, len, &wrote, nullptr);
#else
    assert(_fd >= 0);
    return ::write(_fd, buf, len) >= 0;
#endif
}

void SaveFile::cancel() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    if (INVALID_HANDLE_VALUE == _handle)
        return;
    ::CloseHandle(_handle);
    _handle = INVALID_HANDLE_VALUE;
    OS::removefile(_tmp_path);
#else
    if (_fd < 0)
        return;
    ::close(_fd);
    _fd = -1;
    OS::removefile(_tmp_path);
#endif
}

bool SaveFile::commit() noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    assert(INVALID_HANDLE_VALUE != _handle);
    ::FlushFileBuffers(_handle);
    ::CloseHandle(_handle);
    _handle = INVALID_HANDLE_VALUE;
#else
    assert(_fd >= 0);
    // NOTE 确保在重命名文件之前数据全部写入磁盘
    //      参见 http://www.tuicool.com/articles/BjA7JjQ
    ::fsync(_fd);
    ::close(_fd);
    _fd = -1;
#endif

    // NOTE rename() 是一个原子操作，能够保证被移动的数据不会丢失
    if (Path::exists(_path) && !OS::removefile(_path))
        return false;
    if (!OS::rename(_tmp_path, _path))
        return false;

#if !NUT_PLATFORM_OS_WINDOWS
    const std::string parentdir = Path::dirname(_path);
    const int dirfd = ::open(parentdir.c_str(), O_DIRECTORY | O_RDONLY);
    ::fsync(dirfd);
    ::close(dirfd);
#endif

    return true;
}

}
