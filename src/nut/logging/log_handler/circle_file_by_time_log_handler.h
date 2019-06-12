
#ifndef ___HEADFILE_188D63C6_448B_4D88_A912_578978532F69_
#define ___HEADFILE_188D63C6_448B_4D88_A912_578978532F69_

#include <string>
#include <fstream>

#include "../../nut_config.h"
#include "log_handler.h"


namespace nut
{

/**
 * 按照日志文件创建的时间, 自动循环使用一定数量的日志文件来记录
 */
class NUT_API CircleFileByTimeLogHandler : public LogHandler
{
public:
    /**
     * 按照创建时间循环
     *
     * @param dir_path 日志文件所在的目录
     * @param prefix 日志文件前缀
     * @param circle_size 循环周期(最多日志文件数)
     */
    CircleFileByTimeLogHandler(const std::string& dir_path,
                               const std::string& prefix, size_t circle_size) noexcept;

    virtual void handle_log(const LogRecord& rec) noexcept override;

private:
    CircleFileByTimeLogHandler(const CircleFileByTimeLogHandler&) = delete;
    CircleFileByTimeLogHandler& operator=(const CircleFileByTimeLogHandler&) = delete;

private:
    std::ofstream _ofs;
};

}

#endif
