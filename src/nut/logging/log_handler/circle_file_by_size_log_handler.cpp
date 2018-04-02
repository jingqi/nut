
#include <math.h>

#include <nut/platform/os.h>
#include <nut/platform/path.h>
#include <nut/util/string/to_string.h>
#include <nut/util/string/string_util.h>

#include "circle_file_by_size_log_handler.h"

// 数字序列长度
#define SEQ_NUM_LENGTH 5
// 数字序列周期
#define SEQ_ROUND ((long) ::pow(10, SEQ_NUM_LENGTH))

namespace nut
{

CircleFileBySizeLogHandler::CircleFileBySizeLogHandler(const std::string& dir_path,
        const std::string& prefix, size_t circle_size, size_t max_file_size, bool cross_file)
    : _dir_path(dir_path), _file_prefix(prefix), _circle_size(circle_size),
      _max_file_size(max_file_size), _cross_file(cross_file)
{
    assert(0 < _circle_size && _circle_size <= SEQ_ROUND);
    circle_once();
}

void CircleFileBySizeLogHandler::reopen(const char *file)
{
    assert(nullptr != file);

    if (Path::exists(file))
        _file_size = Path::get_size(file);
    else
        _file_size = 0;

    _ofs.close();
    _ofs.clear();
    _ofs.open(file, std::ios::app);

    const char *first_msg = "\n\n\n\n\n\n------------- ---------------- ---------------\n";
    _ofs << first_msg;
    _file_size += ::strlen(first_msg);
}

void CircleFileBySizeLogHandler::circle_once()
{
    // 关闭之前打开的文件, 强制刷新磁盘, 避免获取文件大小的结果不准确
    _ofs.close();

    // 找到相同目录下所有的日志文件
    const std::string log_suffix(".log");
    std::vector<std::string> file_names, logfile_names;
    OS::list_dir(_dir_path, &file_names, false, true, true);

    const size_t prefix_len = _file_prefix.length();
    for (size_t i = 0, sz = file_names.size(); i < sz; ++i)
    {
        const std::string& name = file_names.at(i);

        // 前缀、后缀
        if (!starts_with(name, _file_prefix))
            continue;
        if (!ends_with(name, log_suffix))
            continue;

        // 名称长度
        if (name.length() != prefix_len + SEQ_NUM_LENGTH + log_suffix.length())
            return;

        // 数字序列验证
        bool all_num = true;
        for (size_t j = 0; j < SEQ_NUM_LENGTH; ++j)
        {
            const char c = name.at(prefix_len + j);
            if (c < '0' || c > '9')
            {
                all_num = false;
                break;
            }
        }
        if (!all_num)
            continue;

        logfile_names.push_back(name);
    }

    // 检查最后一个文件是否能够容纳更多日志
    std::sort(logfile_names.begin(), logfile_names.end());
    long next_seq = 0;
    std::string next_file_name; // If non-empty, use the last file
    if (!logfile_names.empty())
    {
        const std::string& last_name = *logfile_names.rbegin();
        next_seq = str_to_long(last_name.substr(prefix_len, SEQ_NUM_LENGTH)) + 1;

        const long long filesz = Path::get_size(Path::join(_dir_path, last_name));
        if (filesz < _max_file_size)
            next_file_name = last_name;
    }

    // 删除多余的日志文件
    size_t keep_count = _circle_size - (next_file_name.empty() ? 1 : 0);
    if (next_seq >= SEQ_ROUND)
    {
        // 超过了最大数目，全部清空
        keep_count = 0;
        next_seq = 0;
    }
    if (logfile_names.size() > keep_count)
    {
        for (size_t i = 0, del_count = logfile_names.size() - keep_count;
             i < del_count; ++i)
        {
            std::string full_path;
            Path::join(_dir_path, logfile_names.at(i), &full_path);
            OS::remove_file(full_path);
        }
    }

    // 构建日志文件名
    if (next_file_name.empty())
    {
        next_file_name = _file_prefix;
        const std::string seq_num = long_to_str(next_seq);
        for (size_t i = seq_num.length(); i < SEQ_NUM_LENGTH; ++i)
            next_file_name += '0';
        next_file_name += seq_num;
        next_file_name += log_suffix;
    }

    // 打开日志文件
    std::string full_path;
    Path::join(_dir_path, next_file_name, &full_path);
    reopen(full_path.c_str());
}

void CircleFileBySizeLogHandler::handle_log(const LogRecord& rec)
{
    // Write log record
    const std::string msg = rec.to_string();
    _ofs << msg << std::endl;
    _file_size += msg.length() + 1;

    // Flush to disk if needed
    if (0 != (_flush_mask & static_cast<loglevel_mask_type>(rec.get_level())))
        _ofs.flush();

    // Change to new log file if needed
    if (_cross_file && _file_size >= _max_file_size)
        circle_once();
}

}
