
#ifndef ___HEADFILE_62C1DBA1_286E_4BAE_AD7D_FD628348E94C_
#define ___HEADFILE_62C1DBA1_286E_4BAE_AD7D_FD628348E94C_

#include <fstream>
#include <mutex>

#include "../../nut_config.h"
#include "../../platform/platform.h"
#include "../../threading/sync/spin_lock.h"
#include "log_handler.h"


namespace nut
{

class NUT_API FileLogHandler : public LogHandler
{
public:
    /**
     * @param trunc 先清空日志文件，然后再写入
     */
    explicit FileLogHandler(const char *file, bool trunc = false) noexcept;

    virtual void handle_log(const LogRecord& rec) noexcept override;

private:
    FileLogHandler(const FileLogHandler&) = delete;
    FileLogHandler& operator=(const FileLogHandler&) = delete;

private:
#if NUT_PLATFORM_OS_WINDOWS
    std::ofstream _ofs;
    SpinLock _ofs_lock;
#else
    int _fd = -1;
#endif
};


}

#endif
