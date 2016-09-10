
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h> // for GetCurrentProcessId()
#else
#   include <unistd.h> // for getpid()
#endif

#include <nut/platform/os.h>
#include <nut/platform/path.h>
#include <nut/util/time/date_time.h>
#include <nut/util/string/to_string.h>
#include <nut/util/string/string_util.h>

#include "circle_file_by_time_log_handler.h"

namespace nut
{

CircleFileByTimeLogHandler::CircleFileByTimeLogHandler(const std::string& dir_path,
        const std::string& prefix, size_t circle_size)
{
    assert(circle_size > 0);

    // 找到相同目录下所有的日志文件
    const std::string log_suffix(".log");
    std::vector<std::string> file_names, logfile_names;
    OS::list_dir(dir_path, &file_names, false, true, true);

    for (size_t i = 0; i < file_names.size(); ++i)
    {
        const std::string& name = file_names.at(i);

        // 前缀、后缀
        if (!ends_with(name, log_suffix))
            continue;
        if (!starts_with(name, prefix))
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
            std::string full;
            Path::join(dir_path, logfile_names.at(i), &full);
            OS::remove_file(full);
        }
    }

    // 构建日志文件名
    std::string file_name(prefix);
    file_name += DateTime().format_time("%Y-%m-%d %H-%M-%S ");
#if NUT_PLATFORM_OS_WINDOWS
    long pid = ::GetCurrentProcessId();
#else
    pid_t pid = ::getpid();
#endif
    llong_to_str(pid, &file_name);
    file_name += log_suffix;

    std::string full_path;
    Path::join(dir_path, file_name);
    _ofs.open(full_path.c_str(), std::ios::trunc);
}

void CircleFileByTimeLogHandler::handle_log(const LogRecord& rec)
{
    _ofs << rec.to_string() << std::endl;

    if (0 != (_flush_mask & rec.get_level()))
        _ofs.flush();
}

}
