
#ifndef ___HEADFILE_62C1DBA1_286E_4BAE_AD7D_FD628348E94C_
#define ___HEADFILE_62C1DBA1_286E_4BAE_AD7D_FD628348E94C_

#include <fstream>

#include "../../nut_config.h"
#include "log_handler.h"


namespace nut
{

class NUT_API FileLogHandler : public LogHandler
{
public:
    /**
     * @param append 是否是追加模式。追加模式日志文件支持并发写
     */
    explicit FileLogHandler(const char *file, bool append = true) noexcept;

    virtual void handle_log(const LogRecord& rec) noexcept override;

private:
    FileLogHandler(const FileLogHandler&) = delete;
    FileLogHandler& operator=(const FileLogHandler&) = delete;

private:
    std::ofstream _ofs;
};


}

#endif
