
#include <assert.h>
#include <fcntl.h> // for ::open()
#include <sys/stat.h> // for ::lstat()

#include "platform.h"
#include "path.h"
#include "os.h"
#include "savefile.h"


namespace nut
{

SaveFile::SaveFile(const std::string& path) noexcept
    : _path(Path::realpath(path))
{
    _tmp_path = _path + ".tmp";
}

SaveFile::~SaveFile() noexcept
{
    if (FD::is_valid_fd(_fd))
        commit();
}

bool SaveFile::open() noexcept
{
    assert(!FD::is_valid_fd(_fd));
#if NUT_PLATFORM_OS_WINDOWS
    _fd = ::CreateFileA(_tmp_path.c_str(), GENERIC_WRITE, 0, nullptr,
                        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
#else
    // 要覆盖的文件不能是一个目录
    struct stat info;
    if (0 == ::lstat(_path.c_str(), &info) && !S_ISREG(info.st_mode))
        return false;

    _fd = ::open(_tmp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW, 0644);
#endif
    return FD::is_valid_fd(_fd);
}

bool SaveFile::write(const void *buf, size_t len) noexcept
{
    assert(nullptr != buf || 0 == len);
    if (nullptr == buf || 0 == len)
        return true;

    return FD::write(_fd, buf, len) >= 0;
}

void SaveFile::cancel() noexcept
{
    if (!FD::is_valid_fd(_fd))
        return;
    FD::close(_fd);
    _fd = NUT_INVALID_FD;
    OS::removefile(_tmp_path);
}

bool SaveFile::commit() noexcept
{
    assert(FD::is_valid_fd(_fd));

    // NOTE 确保在重命名文件之前数据全部写入磁盘
    //      参见 http://www.tuicool.com/articles/BjA7JjQ
    FD::fsync(_fd);
    FD::close(_fd);
    _fd = NUT_INVALID_FD;

    // NOTE rename() 是一个原子操作，能够保证被移动的数据不会丢失
    if (Path::exists(_path) && !OS::removefile(_path))
        return false;
    if (!OS::rename(_tmp_path, _path))
        return false;

#if !NUT_PLATFORM_OS_WINDOWS
    const std::string parentdir = Path::dirname(_path);
    const int dirfd = ::open(parentdir.c_str(), O_DIRECTORY | O_RDONLY);
    FD::fsync(dirfd);
    FD::close(dirfd);
#endif

    return true;
}

}
