
#ifndef ___HEADFILE_188D63C6_448B_4D88_A912_578978532F69_
#define ___HEADFILE_188D63C6_448B_4D88_A912_578978532F69_

#include <string>
#include <fstream>

#include "../../nut_config.h"
#include "../../platform/platform.h"
#include "../../threading/sync/spin_lock.h"
#include "log_handler.h"


namespace nut
{

/**
 * 按照日志文件创建的时间, 自动循环使用一定数量的日志文件来记录
 */
class NUT_API CircleFilesByTimeLogHandler : public LogHandler
{
public:
    /**
     * 按照创建时间循环
     *
     * 如果多次运行时生成的文件名相同, 新日志将会附加到文件末尾, 这种特性可以用来
     * 比如说将同一天的日志存放到同一个文件中.
     *
     * @param dir_path 日志文件所在的目录
     * @param prefix 日志文件名前缀
     * @param time_format 日志文件名日期格式, 例如 "%Y%m%d-%H%M%S-%3f"
     * @param circle_size 循环周期(最多日志文件数)
     */
    CircleFilesByTimeLogHandler(const std::string& dir_path, const std::string& prefix,
                                const char *time_format, size_t circle_size) noexcept;

    virtual void handle_log(const LogRecord& rec) noexcept override;

private:
    CircleFilesByTimeLogHandler(const CircleFilesByTimeLogHandler&) = delete;
    CircleFilesByTimeLogHandler& operator=(const CircleFilesByTimeLogHandler&) = delete;

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
