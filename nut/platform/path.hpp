/**
 * @file -
 * @author jingqi
 * @date 2012-06-23
 * @last-edit 2012-06-23 15:38:33 jingqi
 */

#ifndef ___HEADFILE_E6D40B10_E5D6_4092_A38B_4E69E5B8E123_
#define ___HEADFILE_E6D40B10_E5D6_4092_A38B_4E69E5B8E123_

#include <assert.h>
#include <string>
#include <vector>

#include "platform.hpp"

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#   include<io.h> // for _access()
#   include <sys/stat.h> // for stat()
#else
#   include <unistd.h> // for access()
#   include <sys/stat.h> // for stat()
#endif

#include <nut/util/tuple.hpp>

namespace nut
{

class Path
{
private:
    Path() {}

public:
    /**
     * 路径分隔符
     */
    static inline char seperator()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return '\\';
#else
        return '/';
#endif
    }

    /**
     * 检查字符是否是路径分隔符 '\\' '/'
     */
    static inline bool isPathSeparator(char c)
    {
        return '\\' == c || '/' == c;
    }

    // static std::string abspath(const std::string& p) {}
    // static std::string realpath() {}
    // static std::string relpath() {}

    // static std::string sep() {}

    /**
     * 从路径中划分出父路径和 文件/文件夹 名
     *
     * 例如：
     * "ab/c/d" -> "ab/c" "d"
     * "/ab.txt" -> "/" "ab.txt"
     * "c:\\tmp" -> "c:\\" "tmp"
     */
    static Tuple<std::string,std::string> split(const char *path)
    {
        assert(NULL != path);
        
        // 找到最后一个 '/'
        const std::string p(path);
        std::string::size_type p1 = p.find_last_of('\\'), p2 = p.find_last_of('/');
        if (std::string::npos == p1 && std::string::npos == p2)
            return Tuple<std::string, std::string>("", p);
        else if (std::string::npos != p1 && std::string::npos != p2)
            p1 = (p1 > p2 ? p1 : p2);
        else if (std::string::npos == p1)
            p1 = p2;

        assert(p1 >= 0 && std::string::npos != p1);
        if (0 == p1 || ':' == path[p1 - 1]) // 磁盘号 + 根目录
            return Tuple<std::string, std::string>(p.substr(0, p1 + 1), p.substr(p1 + 1));
        else
            return Tuple<std::string, std::string>(p.substr(0, p1), p.substr(p1 + 1));
    }

    /**
     * 从路径中划分出磁盘号和路径(linux路径的磁盘号假定为"")
     *
     * 例如：
     * "c:\\mn\\p" -> "c:" "\\mn\\p"
     * "/mnt/sdcard" -> "" "/mnt/sdcard"
     */
    static Tuple<std::string,std::string> splitdrive(const char *path)
    {
        assert(NULL != path);
        int pos = -1;
        for (int i = 0; '\0' != path[i]; ++i)
        {
            if ('\\' == path[i] || '/' == path[i])
                break;
            if (':' == path[i])
            {
                pos = i;
                break;
            }
        }
        if (pos < 0)
            return Tuple<std::string,std::string>("", path);
        const std::string p(path);
        return Tuple<std::string,std::string>(p.substr(0, pos + 1), p.substr(pos + 1));
    }

    /**
     * 从路径或者文件名中分离出后缀名
     *
     * 例如：
     * "a.txt" -> "a" ".txt"
     */
    static Tuple<std::string,std::string> splitext(const char *path)
    {
        assert(NULL != path);
        const std::string p(path);
        int pos = -1;
        for (int i = p.length() - 1; i >= 0; --i)
        {
            if ('\\' == path[i] || '/' == path[i])
                break;
            if ('.' == path[i])
            {
                // 排除 "a/.abc" 的情况
                if (i - 1 >= 0 && '\\' != path[i - 1] && '/' != path[i - 1])
                    pos = i;
                break;
            }
        }
        if (pos < 0)
            return Tuple<std::string,std::string>(p,"");
        return Tuple<std::string,std::string>(p.substr(0,pos),p.substr(pos));
    }

    // static void splitunc() {}

    /**
     * 检查路径是否存在
     */
    static inline bool exists(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return -1 != ::_access(path, 0);
#else
        /*
         *  0-检查文件是否存在
         *  1-检查文件是否可运行
         *  2-检查文件是否可写访问
         *  4-检查文件是否可读访问
         *  6-检查文件是否可读/写访问
         */
        return 0 == ::access(path, 0);
#endif
    }

    /**
     * last access time
     */
    // static time_t getatime() {}

    /**
     * last modified time
     */
    // static time_t getmtime() {}

    /**
     * created time
     */
    // static time_t getctime() {}

    /**
     * 获取文件大小
     */
    static long getsize(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_stat(path, &info);
        return info.st_size;
#else
        struct stat info;
        if (0 != ::stat(path, &info))
            return -1L;
        return info.st_size;
#endif
    }

    // static bool isabs() {}

    static bool isdir(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 != (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesA(path));
#else
        // TODO
#endif
    }

    static bool isfile(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 != (FILE_ATTRIBUTE_NORMAL & ::GetFileAttributesA(path));
#else
        // TODO
#endif
    }

    static bool islink(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return false;
#else
        // TODO
#endif
    }

    // static bool ismount() {}

    /**
     * 连接两个子路径
     *
     * 例如：
     * "a" "b" -> "a/b"
     * "/" "sd" -> "/sd"
     * "c:" "\\tmp" -> "c:\\tmp"
     */
    static std::string join(const char *a, const char *b)
    {
        assert(NULL != a && NULL != b);

        if ('\0' == a[0])
            return b;
            
        // 处理磁盘号
        std::string apath(a);
        if (':' == a[apath.length() - 1])
            return apath + b;

        // 处理根目录 '/', 'c:\\'
        std::string bpath(b);
        if ('\\' == b[0] || '/' == b[0] || std::string::npos != bpath.find_first_of(':'))
            return bpath;

        // 连接
        if ('\\' != *apath.rbegin() && '/' != *apath.rbegin())
            apath += seperator();
        if ('\0' != b[0])
            apath.append(bpath);
        return apath;
    }

    static inline std::string join(const char *a, const char *b, const char *c)
    {
        assert(NULL != a && NULL != b && NULL != c);
        return join(join(a, b).c_str(),c);
    }

    static inline std::string join(const char *a, const char *b, const char *c, const char *d)
    {
        assert(NULL != a && NULL != b && NULL != c && NULL != d);
        return join(join(a, b, c).c_str(), d);
    }
};

}

#endif

