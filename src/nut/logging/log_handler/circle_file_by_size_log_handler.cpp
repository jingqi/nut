
#include <math.h>
#include <string.h> // for ::strlen()
#include <algorithm> // for std::sort()

#include "../../platform/os.h"
#include "../../platform/path.h"
#include "../../util/string/to_string.h"
#include "../../util/string/string_utils.h"
#include "circle_file_by_size_log_handler.h"


namespace nut
{

// 计算十进制数字长度
static size_t decimal_digit_count(size_t num)
{
    size_t ret = 0;
    while (num > 0)
    {
        ++ret;
        num /= 10;
    }
    return 0 == ret ? 1 : ret;
}

// 格式化序号到指定长度
//  e.g. format_seq(12, 4) -> "0012"
static std::string format_seq(size_t seq, size_t len)
{
    std::string ret;
    const std::string s = ulong_to_str(seq);
    for (size_t i = s.length(); i < len; ++i)
        ret.push_back('0');
    ret += s;
    return ret;
}

// 判断字串是否全是有数字组成
static bool is_all_digits(const std::string& s, size_t start, size_t len)
{
    for (size_t i = start; i < start + len; ++i)
    {
        const char c = s.at(i);
        if (c < '0' || c > '9')
            return false;
    }
    return true;
}

CircleFileBySizeLogHandler::CircleFileBySizeLogHandler(
    const std::string& dir_path, const std::string& prefix, size_t circle_size,
    size_t max_file_size, bool cross_file) noexcept
    : _dir_path(dir_path), _file_prefix(prefix), _circle_size(circle_size),
      _max_file_size(max_file_size), _cross_file(cross_file)
{
    assert(_circle_size > 0);
    circle_once();
}

void CircleFileBySizeLogHandler::open(const char *file) noexcept
{
    assert(nullptr != file);

    if (Path::exists(file))
        _file_size = Path::get_size(file);
    else
        _file_size = 0;

    _ofs.open(file, std::ios::app); // NOTE append 模式打开的文件支持并发写入

    if (_file_size > 0)
    {
        _ofs << "\n\n";
        _file_size += 2;
    }
    const char *first_msg = "------------- ---------------- ---------------\n";
    _ofs << first_msg;
    _file_size += ::strlen(first_msg);
}

void CircleFileBySizeLogHandler::circle_once() noexcept
{
    // 关闭文件, 强制写入到文件系统, 避免 get_size() 的结果不准确
    _ofs.close();
    _ofs.clear();

    // 找到目录下所有的日志文件
    const size_t prefix_len = _file_prefix.length();
    const size_t digits_len = decimal_digit_count(_circle_size - 1);
    const std::string log_suffix = ".log";
    const std::vector<std::string> file_names = OS::listdir(_dir_path, false, true, true);
    std::vector<std::string> logfile_names;
    for (size_t i = 0, sz = file_names.size(); i < sz; ++i)
    {
        const std::string& name = file_names.at(i);

        // 名称长度
        if (name.length() != prefix_len + digits_len + log_suffix.length())
            continue;

        // 匹配前缀、后缀
        if (!starts_with(name, _file_prefix) || !ends_with(name, log_suffix))
            continue;

        // 数字序列验证
        if (!is_all_digits(name, prefix_len, digits_len))
            continue;

        logfile_names.push_back(name);
    }

    // 检查 0 号文件是否能够容纳更多日志
    const std::string name_zero = _file_prefix + format_seq(0, digits_len) + log_suffix;
    const std::string path_zero = Path::join(_dir_path, name_zero);
    const bool need_shift = (
        Path::exists(path_zero) && Path::get_size(path_zero) >= _max_file_size);

    // 删除多余的日志文件，重命名日志文件
    std::sort(logfile_names.begin(), logfile_names.end());
    for (ssize_t i = logfile_names.size() - 1; i >= 0; --i)
    {
        const std::string& name = logfile_names.at(i);
        const std::string& ori_path = Path::join(_dir_path, name);

        size_t seq = (size_t) str_to_long(name.substr(prefix_len, digits_len));
        if (need_shift)
            ++seq; // Next seq number

        // 删除多余的日志文件
        if (seq >= _circle_size)
        {
            OS::removefile(ori_path);
            continue;
        }

        // 文件名中的序号加1
        if (need_shift)
        {
            const std::string new_name = _file_prefix + format_seq(seq, digits_len) + log_suffix;
            OS::rename(ori_path, Path::join(_dir_path, new_name));
            continue;
        }

        break;
    }

    // 打开日志文件
    open(path_zero.c_str());
}

void CircleFileBySizeLogHandler::handle_log(const LogRecord& rec) noexcept
{
    // Write log record
    const std::string msg = rec.to_string();
    _ofs << msg << std::endl;
    _file_size += msg.length() + 1;

    // Flush to disk if needed
    if (0 != (_flush_mask & rec.get_level()))
        _ofs.flush();

    // Change to new log file if needed
    if (_cross_file && _file_size >= _max_file_size)
        circle_once();
}

}
