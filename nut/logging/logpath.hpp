/**
 * @file -
 * @author jingqi
 * @date 2011-12-24
 * @last-edit 2011-12-24 15:00:22 jingqi
 */

#ifndef ___HEADFILE_F84AA3B6_A83C_4637_A10F_916FEB7D0E6C_
#define ___HEADFILE_F84AA3B6_A83C_4637_A10F_916FEB7D0E6C_

#include <string>

namespace nut
{

class LogPath
{
public:
    /** 获取日志路径名称, "a.b.c" -> "c" */
    static std::string getName(const std::string& logPath)
    {
        const std::string::size_type i = logPath.find_last_of('.');
        return logPath.substr(std::string::npos == i ? 0 : i + 1);
    }

    /** 获取第一个父名, "a.b.c" -> "a" */
    static std::string getFirstParent(const std::string& logPath)
    {
        const std::string::size_type i = logPath.find_first_of('.');
        return logPath.substr(0, i);
    }

    /** 获取子路径, "a.b.c" -> "b.c" */
    static std::string subLogPath(const std::string& logPath)
    {
        const std::string::size_type i = logPath.find_first_of('.');
        return logPath.substr(std::string::npos == i ? logPath.length() : i + 1);
    }
};

}

#endif

