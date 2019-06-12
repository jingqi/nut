
#ifndef ___HEADFILE_0A0A8477_9AD5_4F0A_ABF5_AEBA0DEF85AF_
#define ___HEADFILE_0A0A8477_9AD5_4F0A_ABF5_AEBA0DEF85AF_

#include <string>
#include <fstream>

#include "../../nut_config.h"
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
     * @param prefix 日志文件前缀
     * @param circle_size 循环周期(最多日志文件数)
     * @param max_file_size 最大文件大小
     * @param cross_file 单次启动记录的日志允许跨越多个文件
     */
    CircleFileBySizeLogHandler(const std::string& dir_path, const std::string& prefix,
                               size_t circle_size, size_t max_file_size,
                               bool cross_file = true) noexcept;

    virtual void handle_log(const LogRecord& rec) noexcept override;

private:
    CircleFileBySizeLogHandler(const CircleFileBySizeLogHandler&) = delete;
    CircleFileBySizeLogHandler& operator=(const CircleFileBySizeLogHandler&) = delete;

    // 重新打开输出流
    void reopen(const char *file) noexcept;

    // 新一轮循环
    void circle_once() noexcept;

private:
    // 输出流
    std::ofstream _ofs;
    // 当前文件大小
    long long _file_size = 0;

    // 输出目录
    std::string _dir_path;
    // 文件名前缀
    std::string _file_prefix;
    // 循环周期
    size_t _circle_size = 1;
    // 最大文件大小
    long long _max_file_size = 0;
    // 是否允许跨越文件
    bool _cross_file = true;
};

}

#endif
