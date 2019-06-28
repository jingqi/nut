
#ifndef ___HEADFILE_0A0A8477_9AD5_4F0A_ABF5_AEBA0DEF85AF_
#define ___HEADFILE_0A0A8477_9AD5_4F0A_ABF5_AEBA0DEF85AF_

#include <string>
#include <mutex>
#include <fstream>
#include <atomic>

#include "../../nut_config.h"
#include "../../platform/platform.h"
#include "../../threading/sync/nano_lock.h"
#include "../../threading/sync/spin_lock.h"
#include "log_handler.h"


namespace nut
{

/**
 * 按照单个日志文件大小限制, 自动循环使用一定数量的日志文件来记录
 */
class NUT_API CircleFileBySizeLogHandler : public LogHandler
{
public:
    /**
     * 按照文件大小循环
     *
     * @param dir_path 日志文件所在的目录
     * @param prefix 日志文件名前缀
     * @param circle_size 循环周期(最多日志文件数)
     * @param max_file_size 最大文件大小
     * @param cross_file 单次启动记录的日志允许跨越多个文件
     */
    CircleFileBySizeLogHandler(const std::string& dir_path, const std::string& prefix,
                               size_t circle_size, long long max_file_size,
                               bool cross_file = true) noexcept;

#if !NUT_PLATFORM_OS_WINDOWS
    ~CircleFileBySizeLogHandler();
#endif

    virtual void handle_log(const LogRecord& rec) noexcept override;

private:
    CircleFileBySizeLogHandler(const CircleFileBySizeLogHandler&) = delete;
    CircleFileBySizeLogHandler& operator=(const CircleFileBySizeLogHandler&) = delete;

    // 打开文件输出流
    void open_log_file(const std::string& file) noexcept;

    // 新一轮循环
    void circle_once() noexcept;

private:
#if NUT_PLATFORM_OS_WINDOWS
    // 输出流
    std::ofstream _ofs;
    // 输出流锁
    SpinLock _ofs_lock;
#else
    // 日志文件
    std::atomic<int> _fd = ATOMIC_VAR_INIT(-1);
#endif

    // 当前文件大小
    std::atomic<long long> _file_size = ATOMIC_VAR_INIT(0);

    // 文件操作锁
    NanoLock _fileop_lock;

    // 输出目录
    const std::string _dir_path;
    // 文件名前缀
    const std::string _file_prefix;
    // 循环周期
    const size_t _circle_size = 1;
    // 最大文件大小
    const long long _max_file_size = 0;
    // 是否允许跨越文件
    const bool _cross_file = true;
};

}

#endif
