/**
 * @file -
 * @author jingqi
 * @date 2012-06-23
 * @last-edit 2012-11-13 21:30:25 jingqi
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
#   include <direct.h> // for getcwd()
#else
#   include <unistd.h> // for access(), getcwd()
#   include <sys/stat.h> // for stat()
#endif

#include <nut/util/tuple.hpp>
#include <nut/util/string/stringutil.hpp>

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

    static inline wchar_t wseperator()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return L'\\';
#else
        return L'/';
#endif
    }

    /**
     * 检查字符是否是路径分隔符 '\\' '/'
     */
    static inline bool isPathSeparator(char c)
    {
        return '\\' == c || '/' == c;
    }

    static inline bool isPathSeparator(wchar_t c)
    {
        return L'\\' == c || L'/' == c;
    }

    static std::string getCwd()
    {
        char buf[MAX_PATH + 1];
        buf[0] = 0;
        getcwd(buf, MAX_PATH + 1);
        return buf;
    }

    static std::wstring getWCwd()
    {
        return str2wstr(getCwd());
    }

    static std::string abspath(const std::string&  p)
    {
        if (p.empty())
            return getCwd();

        // 探测是否从根路径开始
        bool from_root = false;
        if (p.at(0) == '/') // linux root
        {
            from_root = true;
        }
        else
        {
            for (register size_t i = 0, len = p.length(); i < len; ++i)
            {
                const char c = p.at(i);
                if (c == ':') // windows partion root
                {
                    from_root = true;
                    break;
                }
                else if (isPathSeparator(c))
                {
                    break;
                }
            }
        }
        std::string ret;
        if (!from_root)
            ret = getCwd();

        // 组装路径
        std::string part;
        for (register size_t i = 0, len = p.length(); i < len; ++i)
        {
            const char c = p.at(i);
            if (!isPathSeparator(c))
            {
                part.push_back(c);
                if (i != len - 1)
                    continue;
            }

            // 处理根目录
            if (ret.empty())
            {
                if (part.empty())
                {
                    // linux root
                    ret.push_back('/');
                    continue;
                }
                else if (part.at(part.length() - 1) == ':')
                {
                    // windows partition root
                    ret += part;
                    ret.push_back('\\');
                    part.clear();
                    continue;
                }
            }

            // 组装
            if (!part.empty())
            {
                if (part == "..")
                {
                    register int j = ret.size() - 1;
                    if (isPathSeparator(ret.at(j)))
                        --j;
                    while (j >= 0)
                    {
                        if (isPathSeparator(ret.at(j)))
                            break;
                        --j;
                    }
                    if (j == 0 || (j > 0 && ret.at(j - 1) == ':'))
                        ret.resize(j + 1);
                    else if(j > 0)
                        ret.resize(j);
                    // else: parent directory out of range
                }
                else if (part != ".")
                {
                    if (!ret.empty() && !isPathSeparator(ret.at(ret.length() - 1)))
                    {
                        ret.push_back(seperator());
                    }
                    ret += part;
                }
                part.clear();
            }
        }

        return ret;
    }

    static std::wstring abspath(const std::wstring&  p)
    {
        if (p.empty())
            return getWCwd();

        // 探测是否从根路径开始
        bool from_root = false;
        if (p.at(0) == L'/') // linux root
        {
            from_root = true;
        }
        else
        {
            for (register size_t i = 0, len = p.length(); i < len; ++i)
            {
                const wchar_t c = p.at(i);
                if (c == L':') // windows partion root
                {
                    from_root = true;
                    break;
                }
                else if (isPathSeparator(c))
                {
                    break;
                }
            }
        }
        std::wstring ret;
        if (!from_root)
            ret = getWCwd();

        // 组装路径
        std::wstring part;
        for (register size_t i = 0, len = p.length(); i < len; ++i)
        {
            const wchar_t c = p.at(i);
            if (!isPathSeparator(c))
            {
                part.push_back(c);
                if (i != len - 1)
                    continue;
            }

            // 处理根目录
            if (ret.empty())
            {
                if (part.empty())
                {
                    // linux root
                    ret.push_back(L'/');
                    continue;
                }
                else if (part.at(part.length() - 1) == L':')
                {
                    // windows partition root
                    ret += part;
                    ret.push_back(L'\\');
                    part.clear();
                    continue;
                }
            }

            // 组装
            if (!part.empty())
            {
                if (part == L"..")
                {
                    register int j = ret.size() - 1;
                    if (isPathSeparator(ret.at(j)))
                        --j;
                    while (j >= 0)
                    {
                        if (isPathSeparator(ret.at(j)))
                            break;
                        --j;
                    }
                    if (j == 0 || (j > 0 && ret.at(j - 1) == L':'))
                        ret.resize(j + 1);
                    else if(j > 0)
                        ret.resize(j);
                    // else: parent directory out of range
                }
                else if (part != L".")
                {
                    if (!ret.empty() && !isPathSeparator(ret.at(ret.length() - 1)))
                    {
                        ret.push_back(seperator());
                    }
                    ret += part;
                }
                part.clear();
            }
        }

        return ret;
    }

    // TODO static std::string realpath() {}
    // TODO static std::string relpath() {}

    // TODO static std::string sep() {}

    /**
     * 从路径中划分出父路径和 文件/文件夹 名
     *
     * 例如：
     * "ab/c/d" -> "ab/c" "d"
     * "/ab.txt" -> "/" "ab.txt"
     * "c:\\tmp" -> "c:\\" "tmp"
     */
    static Tuple<std::string,std::string> split(const std::string &path)
    {
        // 找到最后一个 '/'
        std::string::size_type p1 = path.find_last_of('\\'), p2 = path.find_last_of('/');
        if (std::string::npos == p1 && std::string::npos == p2)
            return Tuple<std::string, std::string>("", path);
        else if (std::string::npos != p1 && std::string::npos != p2)
            p1 = (p1 > p2 ? p1 : p2);
        else if (std::string::npos == p1)
            p1 = p2;

        assert(std::string::npos != p1);
        if (0 == p1 || ':' == path[p1 - 1]) // 磁盘号 + 根目录
            return Tuple<std::string, std::string>(path.substr(0, p1 + 1), path.substr(p1 + 1));
        else
            return Tuple<std::string, std::string>(path.substr(0, p1), path.substr(p1 + 1));
    }

    static Tuple<std::wstring,std::wstring> split(const std::wstring& path)
    {
        // 找到最后一个 '/'
        std::wstring::size_type p1 = path.find_last_of(L'\\'), p2 = path.find_last_of(L'/');
        if (std::wstring::npos == p1 && std::wstring::npos == p2)
            return Tuple<std::wstring, std::wstring>(L"", path);
        else if (std::wstring::npos != p1 && std::wstring::npos != p2)
            p1 = (p1 > p2 ? p1 : p2);
        else if (std::wstring::npos == p1)
            p1 = p2;

        assert(std::wstring::npos != p1);
        if (0 == p1 || L':' == path[p1 - 1]) // 磁盘号 + 根目录
            return Tuple<std::wstring, std::wstring>(path.substr(0, p1 + 1), path.substr(p1 + 1));
        else
            return Tuple<std::wstring, std::wstring>(path.substr(0, p1), path.substr(p1 + 1));
    }

    /**
     * 从路径中划分出磁盘号和路径(linux路径的磁盘号假定为"")
     *
     * 例如：
     * "c:\\mn\\p" -> "c:" "\\mn\\p"
     * "/mnt/sdcard" -> "" "/mnt/sdcard"
     */
    static Tuple<std::string,std::string> splitdrive(const std::string& path)
    {
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
        return Tuple<std::string,std::string>(path.substr(0, pos + 1), path.substr(pos + 1));
    }

    static Tuple<std::wstring,std::wstring> splitdrive(const std::wstring& path)
    {
        int pos = -1;
        for (int i = 0; L'\0' != path[i]; ++i)
        {
            if (L'\\' == path[i] || L'/' == path[i])
                break;
            if (L':' == path[i])
            {
                pos = i;
                break;
            }
        }
        if (pos < 0)
            return Tuple<std::wstring,std::wstring>(L"", path);
        return Tuple<std::wstring,std::wstring>(path.substr(0, pos + 1), path.substr(pos + 1));
    }

    /**
     * 从路径或者文件名中分离出后缀名
     *
     * 例如：
     * "a.txt" -> "a" ".txt"
     */
    static Tuple<std::string,std::string> splitext(const std::string& path)
    {
        int pos = -1;
        for (int i = path.length() - 1; i >= 0; --i)
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
            return Tuple<std::string,std::string>(path,"");
        return Tuple<std::string,std::string>(path.substr(0,pos),path.substr(pos));
    }

    static Tuple<std::wstring,std::wstring> splitext(const std::wstring& path)
    {
        int pos = -1;
        for (int i = path.length() - 1; i >= 0; --i)
        {
            if (L'\\' == path[i] || L'/' == path[i])
                break;
            if (L'.' == path[i])
            {
                // 排除 "a/.abc" 的情况
                if (i - 1 >= 0 && L'\\' != path[i - 1] && L'/' != path[i - 1])
                    pos = i;
                break;
            }
        }
        if (pos < 0)
            return Tuple<std::wstring,std::wstring>(path, L"");
        return Tuple<std::wstring,std::wstring>(path.substr(0, pos),path.substr(pos));
    }

    // TODO static void splitunc() {}

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

    static inline bool exists(const std::string& path) { return exists(path.c_str()); }

    static inline bool exists(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return -1 != ::_waccess(path, 0);
#else
        const std::string p = wstr2str(path);
        return exists(p.c_str());
#endif
    }

    static inline bool exists(const std::wstring& path) { return exists(path.c_str()); }

    /**
     * last access time
     */
    static time_t getatime(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_stat(path, &info);
        return info.st_atime;
#else
        struct stat info;
        if (0 != ::stat(path, &info))
            return 0;
        return info.st_atime;
#endif
    }

    static inline time_t getatime(const std::string& path) { return getatime(path.c_str()); }

    static time_t getatime(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_atime;
#else
        const std::string p = wstr2str(path);
        return getatime(p.c_str());
#endif
    }

    static inline time_t getatime(const std::wstring& path) { return getatime(path.c_str()); }


    /**
     * last modified time
     */
    static time_t getmtime(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_stat(path, &info);
        return info.st_mtime;
#else
        struct stat info;
        if (0 != ::stat(path, &info))
            return 0;
        return info.st_mtime;
#endif
    }

    static inline time_t getmtime(const std::string& path) { return getmtime(path.c_str()); }

    static time_t getmtime(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_mtime;
#else
        const std::string p = wstr2str(path);
        return getmtime(p.c_str());
#endif
    }

    static inline time_t getmtime(const std::wstring& path) { return getmtime(path.c_str()); }

    /**
     * created time
     */
    static time_t getctime(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_stat(path, &info);
        return info.st_ctime;
#else
        struct stat info;
        if (0 != ::stat(path, &info))
            return 0;
        return info.st_ctime;
#endif
    }

    static inline time_t getctime(const std::string& path) { return getctime(path.c_str()); }

    static time_t getctime(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_ctime;
#else
        const std::string p = wstr2str(path);
        return getctime(p.c_str());
#endif
    }

    static inline time_t getctime(const std::wstring& path) { return getctime(path.c_str()); }

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

    static inline long getsize(const std::string& path) { return getsize(path.c_str()); }

    static long getsize(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_size;
#else
        const std::string p = wstr2str(path);
        return getsize(p.c_str());
#endif
    }

    static inline long getsize(const std::wstring& path) { return getsize(path.c_str()); }

    // TODO static bool isabs() {}

    static bool isdir(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 != (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesA(path));
#else
        struct stat info;
        if (0 != ::stat(path, &info))
            return false;
        return S_ISDIR(info.st_mode);
#endif
    }

    static inline bool isdir(const std::string& path) { return isdir(path.c_str()); }

    static bool isdir(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 != (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(path));
#else
        const std::string p = wstr2str(path);
        return isdir(p.c_str());
#endif
    }

    static inline bool isdir(const std::wstring& path) { return isdir(path.c_str()); }

    static bool isfile(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 != (FILE_ATTRIBUTE_NORMAL & ::GetFileAttributesA(path));
#else
        struct stat info;
        if (0 != ::stat(path, &info))
            return false;
        return S_ISREG(info.st_mode); // regular file
#endif
    }

    static inline bool isfile(const std::string& path) { return isfile(path.c_str()); }

    static bool isfile(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 != (FILE_ATTRIBUTE_NORMAL & ::GetFileAttributesW(path));
#else
        const std::string p = wstr2str(path);
        return isfile(p.c_str());
#endif
    }

    static inline bool isfile(const std::wstring& path) { return isfile(path.c_str()); }

    static bool islink(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return false;
#else
        struct stat info;
        if (0 != ::stat(path, &info))
            return false;
        return S_ISLNK(info.st_mode);
#endif
    }

    static inline bool islink(const std::string& path) { return islink(path.c_str()); };

    static bool islink(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return false;
#else
        const std::string p = wstr2str(path);
        return isfile(p.c_str());
#endif
    }

    static inline bool islink(const std::wstring& path) { return islink(path.c_str()); }

    // TODO static bool ismount() {}

    /**
     * 连接两个子路径
     *
     * 例如：
     * "a" "b" -> "a/b"
     * "/" "sd" -> "/sd"
     * "c:" "\\tmp" -> "c:\\tmp"
     */
    static std::string join(const std::string& a, const std::string& b)
    {
        if ('\0' == a[0])
            return b;
            
        // 处理磁盘号
        if (':' == *a.rbegin())
            return a + b;

        // 处理根目录 '/', 'c:\\'
        if ('\\' == b[0] || '/' == b[0] || std::string::npos != b.find_first_of(':'))
            return b;

        // 连接
        std::string ret(a);
        if ('\\' != *a.rbegin() && '/' != *a.rbegin())
            ret += seperator();
        if ('\0' != b[0])
            ret.append(b);
        return ret;
    }

    static std::wstring join(const std::wstring& a, const std::wstring& b)
    {
        if (L'\0' == a[0])
            return b;

        // 处理磁盘号
        if (L':' == *a.rbegin())
            return a + b;

        // 处理根目录 '/', 'c:\\'
        if (L'\\' == b[0] || L'/' == b[0] || std::wstring::npos != b.find_first_of(L':'))
            return b;

        // 连接
        std::wstring ret(a);
        if (L'\\' != *a.rbegin() && L'/' != *a.rbegin())
            ret += wseperator();
        if (L'\0' != b[0])
            ret.append(b);
        return ret;
    }

    static inline std::string join(const std::string& a, const std::string& b, const std::string& c)
    {
        return join(join(a, b), c);
    }

    static inline std::wstring join(const std::wstring& a, const std::wstring& b, const std::wstring& c)
    {
        return join(join(a, b),c);
    }

    static inline std::string join(const std::string& a, const std::string& b, const std::string& c, const std::string& d)
    {
        return join(join(a, b, c), d);
    }

    static inline std::wstring join(const std::wstring& a, const std::wstring& b, const std::wstring& c, const std::wstring& d)
    {
        return join(join(a, b, c), d);
    }
};

}

#endif

