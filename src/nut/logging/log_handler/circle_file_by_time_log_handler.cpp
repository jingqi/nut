
#include <algorithm> // for std::sort()

#include "../../platform/os.h"
#include "../../platform/path.h"
#include "../../time/date_time.h"
#include "../../util/string/to_string.h"
#include "../../util/string/string_utils.h"
#include "circle_file_by_time_log_handler.h"


namespace nut
{

CircleFileByTimeLogHandler::CircleFileByTimeLogHandler(
    const std::string& dir_path, const std::string& prefix,
    const char *time_format, size_t circle_size) noexcept
{
    assert(nullptr != time_format && circle_size > 0);

    // 找到目录下所有的日志文件
    const std::string log_suffix(".log");
    const std::vector<std::string> file_names = OS::listdir(dir_path, false, true, true);
    std::vector<std::string> logfile_names;
    for (size_t i = 0; i < file_names.size(); ++i)
    {
        const std::string& name = file_names.at(i);

        // 匹配前缀、后缀
        if (!starts_with(name, prefix) || !ends_with(name, log_suffix))
            continue;

        logfile_names.push_back(name);
    }

    // 删除多余的日志文件
    if (logfile_names.size() > circle_size - 1)
    {
        std::sort(logfile_names.begin(), logfile_names.end());
        for (size_t i = 0, del_count = logfile_names.size() - circle_size + 1;
             i < del_count; ++i)
        {
            const std::string full_path = Path::join(dir_path, logfile_names.at(i));
            OS::removefile(full_path);
        }
    }

    // 构建日志文件名
    std::string file_name(prefix);
    file_name += DateTime::now().format_time(time_format);
    file_name += log_suffix;

    const std::string full_path = Path::join(dir_path, file_name);
    _ofs.open(full_path.c_str(), std::ios::app); // NOTE 'O_APPEND' 模式打开的文件支持并发写
}

void CircleFileByTimeLogHandler::handle_log(const LogRecord& rec) noexcept
{
    _ofs << rec.to_string() << std::endl;

    if (0 != (_flush_mask & rec.get_level()))
        _ofs.flush();
}

}
