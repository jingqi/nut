
#ifndef ___HEADFILE_F84AA3B6_A83C_4637_A10F_916FEB7D0E6C_
#define ___HEADFILE_F84AA3B6_A83C_4637_A10F_916FEB7D0E6C_

#include <string>

namespace nut
{

class LogPath
{
public:
    /**
     * 获取日志路径名称, "a.b.c" -> "c"
     */
    static std::string get_name(const std::string& log_path)
    {
        const std::string::size_type i = log_path.find_last_of('.');
        return log_path.substr(std::string::npos == i ? 0 : i + 1);
    }

    /**
     * 获取第一个父名, "a.b.c" -> "a"
     */
    static std::string get_first_parent(const std::string& log_path)
    {
        const std::string::size_type i = log_path.find_first_of('.');
        return log_path.substr(0, i);
    }

    /**
     * 获取子路径, "a.b.c" -> "b.c"
     */
    static std::string sub_log_path(const std::string& log_path)
    {
        const std::string::size_type i = log_path.find_first_of('.');
        return log_path.substr(std::string::npos == i ? log_path.length() : i + 1);
    }
};

}

#endif
