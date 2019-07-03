
#ifndef ___HEADFILE_CA91379E_6AF7_4A98_AB78_7175A53466FD_
#define ___HEADFILE_CA91379E_6AF7_4A98_AB78_7175A53466FD_

#include "platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include "../nut_config.h"
#include "int_type.h" // for ssize_t


namespace nut
{

#if NUT_PLATFORM_OS_WINDOWS
typedef HANDLE fd_type;
    // INVALID_HANDLE_VALUE is an invalid value returned by ::CreateFile()
#   define NUT_INVALID_FD INVALID_HANDLE_VALUE
#else
typedef int fd_type;
#   define NUT_INVALID_FD -1
#endif

/**
 * Low-level file-descriptor operation
 */
class NUT_API FD
{
public:
    enum class SeekWhence
    {
        FileBegin,
        FileCurrent,
        FileEnd
    };

public:
    // 判断是否是有效的 fd
    static bool is_valid_fd(fd_type fd) noexcept;

    static bool close(fd_type fd) noexcept;

    /**
     * @return 成功则返回新的文件位置; 否则返回 -1
     */
    static long long seek(fd_type fd, long long offset, SeekWhence whence) noexcept;
    static long long tell(fd_type fd) noexcept;

    /**
     * @return 成功则返回读取的字节数; 否则返回 -1
     */
    static ssize_t read(fd_type fd, void *buffer, size_t size) noexcept;

    /**
     * @return 成功则返回读取的字节数; 否则返回 -1
     */
    static ssize_t write(fd_type fd, const void *buffer, size_t size) noexcept;

    /**
     * NOTE
     *    truncate() 之后请重新 seek()
     */
    static bool truncate(fd_type fd, long long size) noexcept;

    static bool fsync(fd_type fd) noexcept;

private:
    FD() = delete;
};

}

#endif
