
#include <assert.h>

#include "platform.h"

#if !NUT_PLATFORM_OS_WINDOWS
#   include <unistd.h> // for read(), write(), close()
#endif

#include "fd.h"


namespace nut
{

bool FD::is_valid_fd(fd_type fd) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    return INVALID_HANDLE_VALUE != fd;
#else
    return fd >= 0;
#endif
}

bool FD::close(fd_type fd) noexcept
{
    assert(is_valid_fd(fd));

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::CloseHandle(fd);
#else
    return 0 == ::close(fd);
#endif
}

long long FD::seek(fd_type fd, long long offset, SeekWhence whence) noexcept
{
    assert(is_valid_fd(fd));

#if NUT_PLATFORM_OS_WINDOWS
    LARGE_INTEGER pos;
    pos.QuadPart = offset;
    const DWORD method = (SeekWhence::FileBegin == whence ? FILE_BEGIN :
                          (SeekWhence::FileCurrent == whence ? FILE_CURRENT : FILE_END));
    const DWORD rs = ::SetFilePointer(fd, pos.LowPart, &pos.HighPart, method);
    if (INVALID_SET_FILE_POINTER == rs)
        return -1;
    pos.LowPart = rs;
    return pos.QuadPart;
#else
    const int method = (SeekWhence::FileBegin == whence ? SEEK_SET :
                        (SeekWhence::FileCurrent == whence ? SEEK_CUR : SEEK_END));
    return ::lseek(fd, offset, method);
#endif
}

long long FD::tell(fd_type fd) noexcept
{
    assert(is_valid_fd(fd));
    return FD::seek(fd, 0, SeekWhence::FileCurrent);
}

ssize_t FD::read(fd_type fd, void *buffer, size_t size) noexcept
{
    assert(is_valid_fd(fd) && nullptr != buffer);

#if NUT_PLATFORM_OS_WINDOWS
    DWORD readed = 0;
    if (FALSE == ::ReadFile(fd, buffer, size, &readed, nullptr))
        return -1;
    return readed;
#else
    return ::read(fd, buffer, size);
#endif
}

ssize_t FD::write(fd_type fd, const void *buffer, size_t size) noexcept
{
    assert(is_valid_fd(fd) && nullptr != buffer);

#if NUT_PLATFORM_OS_WINDOWS
    DWORD wrote = 0;
    if (FALSE == ::WriteFile(fd, buffer, size, &wrote, nullptr))
        return -1;
    return wrote;
#else
    return ::write(fd, buffer, size);
#endif
}

bool FD::truncate(fd_type fd, long long size) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    if (FD::seek(fd, size, SeekWhence::FileBegin) < 0)
        return false;
    return FALSE != ::SetEndOfFile(fd);
#else
    return 0 == ::ftruncate(fd, size);
#endif
}

bool FD::fsync(fd_type fd) noexcept
{
    assert(is_valid_fd(fd));
#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::FlushFileBuffers(fd);
#else
    return 0 == ::fsync(fd);
#endif
}

}
