/**
 * @file -
 * @author jingqi
 * @date 2012-06-23
 * @last-edit 2015-01-06 23:08:26 jingqi
 */

#ifndef ___HEADFILE_E6D40B10_E5D6_4092_A38B_4E69E5B8E123_
#define ___HEADFILE_E6D40B10_E5D6_4092_A38B_4E69E5B8E123_

#include <assert.h>
#include <string>

#include "platform.hpp"

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#   include<io.h> // for _access()
#   include <sys/stat.h> // for stat()
#   include <direct.h> // for getcwd()
#else
#   include <unistd.h> // for access(), getcwd()
#   include <sys/stat.h> // for stat()
#   include <limits.h> // for PATH_MAX
#endif

#include <nut/container/tuple.hpp>
#include <nut/util/string/string_util.hpp>

namespace nut
{

class Path
{
private:
    Path(); // invalid

public:
    /**
     * 路径分隔符
     */
    static char seperator()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return '\\';
#else
        return '/';
#endif
    }

    static wchar_t wseperator()
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
    static bool is_path_separator(char c)
    {
        return '\\' == c || '/' == c;
    }

    static bool is_path_separator(wchar_t c)
    {
        return L'\\' == c || L'/' == c;
    }

    static void get_cwd(std::string *appended)
    {
        assert(NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   if defined(NUT_PLATFORM_CC_VC)
#       pragma warning(push)
#       pragma warning(disable: 4996)
#   endif
        char buf[MAX_PATH + 1];
        buf[0] = 0;
        getcwd(buf, MAX_PATH + 1);
#   if defined(NUT_PLATFORM_CC_VC)
#       pragma warning(pop)
#   endif
#else
        char buf[PATH_MAX + 1];
        buf[0] = 0;
        getcwd(buf, PATH_MAX + 1);
#endif

        *appended += buf;
    }

    static void get_cwd(std::wstring *appended)
    {
        assert(NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   if defined(NUT_PLATFORM_CC_VC)
#       pragma warning(push)
#       pragma warning(disable: 4996)
#   endif
        char buf[MAX_PATH + 1];
        buf[0] = 0;
        getcwd(buf, MAX_PATH + 1);
#   if defined(NUT_PLATFORM_CC_VC)
#       pragma warning(pop)
#   endif
#else
        char buf[PATH_MAX + 1];
        buf[0] = 0;
        getcwd(buf, PATH_MAX + 1);
#endif

        ascii_to_wstr(buf, appended);
    }

    static std::string get_cwd()
    {
        std::string ret;
        get_cwd(&ret);
        return ret;
    }

    static std::wstring get_wcwd()
    {
        std::wstring ret;
        get_cwd(&ret);
        return ret;
    }

    static bool is_abs(const std::string& p)
    {
        if (p.empty())
            return false;

        if (p.at(0) == '/') // linux root
            return true;
        for (size_t i = 0, len = p.length(); i < len; ++i)
        {
            const char c = p.at(i);
            if (c == ':') // windows partion root
                return true;
            else if (is_path_separator(c))
                return false;
        }
        return false;
    }

    static bool is_abs(const std::wstring& p)
    {
        if (p.empty())
            return false;

        if (p.at(0) == L'/') // linux root
            return true;
        for (size_t i = 0, len = p.length(); i < len; ++i)
        {
            const wchar_t c = p.at(i);
            if (c == L':') // windows partion root
                return true;
            else if (is_path_separator(c))
                return false;
        }
        return false;
    }

    static void abs_path(const std::string& p, std::string *appended)
    {
        assert(NULL != appended);
        if (p.empty())
        {
            get_cwd(appended);
            return;
        }

        // 探测是否从根路径开始
        const size_t mark = appended->length();
        if (!is_abs(p))
            get_cwd(appended);

        // 组装路径
        std::string part;
        for (size_t i = 0, len = p.length(); i < len; ++i)
        {
            const char c = p.at(i);
            if (!is_path_separator(c))
            {
                part.push_back(c);
                if (i != len - 1)
                    continue;
            }

            // 处理根目录
            if (appended->length() == mark)
            {
                if (part.empty())
                {
                    // linux root
                    appended->push_back('/');
                    continue;
                }
                else if (part.at(part.length() - 1) == ':')
                {
                    // windows partition root
                    *appended += part;
                    appended->push_back('\\');
                    part.clear();
                    continue;
                }
            }

            // 组装
            if (!part.empty())
            {
                if (part == "..")
                {
                    int j = (int) appended->size() - 1;
                    if (is_path_separator(appended->at(j)))
                        --j;
                    while (j >= (int) mark)
                    {
                        if (is_path_separator(appended->at(j)))
                            break;
                        --j;
                    }
                    if (j == (int) mark || (j > (int) mark && appended->at(j - 1) == ':'))
                        appended->resize(j + 1);
                    else if (j > (int) mark)
                        appended->resize(j);
                    // else: parent directory out of range
                }
                else if (part != ".")
                {
                    if (appended->length() > mark && !is_path_separator(appended->at(appended->length() - 1)))
                    {
                        appended->push_back(seperator());
                    }
                    *appended += part;
                }
                part.clear();
            }
        }
    }

    static void abs_path(const std::wstring& p, std::wstring *appended)
    {
        assert(NULL != appended);
        if (p.empty())
        {
            get_cwd(appended);
            return;
        }

        // 探测是否从根路径开始
        const size_t mark = appended->length();
        if (!is_abs(p))
            get_cwd(appended);

        // 组装路径
        std::wstring part;
        for (size_t i = 0, len = p.length(); i < len; ++i)
        {
            const wchar_t c = p.at(i);
            if (!is_path_separator(c))
            {
                part.push_back(c);
                if (i != len - 1)
                    continue;
            }

            // 处理根目录
            if (appended->length() == mark)
            {
                if (part.empty())
                {
                    // linux root
                    appended->push_back(L'/');
                    continue;
                }
                else if (part.at(part.length() - 1) == L':')
                {
                    // windows partition root
                    *appended += part;
                    appended->push_back(L'\\');
                    part.clear();
                    continue;
                }
            }

            // 组装
            if (!part.empty())
            {
                if (part == L"..")
                {
                    int j = (int) appended->size() - 1;
                    if (is_path_separator(appended->at(j)))
                        --j;
                    while (j >= (int) mark)
                    {
                        if (is_path_separator(appended->at(j)))
                            break;
                        --j;
                    }
                    if (j == (int) mark || (j > (int) mark && appended->at(j - 1) == L':'))
                        appended->resize(j + 1);
                    else if (j > (int) mark)
                        appended->resize(j);
                    // else: parent directory out of range
                }
                else if (part != L".")
                {
                    if (appended->length() > mark && !is_path_separator(appended->at(appended->length() - 1)))
                    {
                        appended->push_back(seperator());
                    }
                    *appended += part;
                }
                part.clear();
            }
        }
    }

    static std::string abs_path(const std::string& p)
    {
        std::string ret;
        abs_path(p, &ret);
        return ret;
    }

    static std::wstring abs_path(const std::wstring& p)
    {
        std::wstring ret;
        abs_path(p, &ret);
        return ret;
    }

    // TODO static std::string relative_path() {}
    // TODO static std::string real_path() {}

    /**
     * 从路径中划分出父路径和 文件/文件夹 名
     *
     * 例如：
     * "ab/c/d" -> "ab/c" "d"
     * "/ab.txt" -> "/" "ab.txt"
     * "c:\\tmp" -> "c:\\" "tmp"
     */
    static void split(const std::string &path, std::string *parent_appended, std::string *child_appended)
    {
        assert(NULL != parent_appended || NULL != child_appended);

        // 找到最后一个 '/'
        std::string::size_type p1 = path.find_last_of('\\'), p2 = path.find_last_of('/');
        if (std::string::npos == p1 && std::string::npos == p2)
        {
            if (NULL != child_appended)
                *child_appended += path;
            return;
        }
        else if (std::string::npos != p1 && std::string::npos != p2)
        {
            p1 = (p1 > p2 ? p1 : p2);
        }
        else if (std::string::npos == p1)
        {
            p1 = p2;
        }

        assert(std::string::npos != p1);
        if (0 == p1 || ':' == path[p1 - 1]) // 磁盘号 + 根目录
        {
            if (NULL != parent_appended)
                *parent_appended += path.substr(0, p1 + 1);
            if (NULL != child_appended)
                *child_appended += path.substr(p1 + 1);
        }
        else
        {
            if (NULL != parent_appended)
                *parent_appended += path.substr(0, p1);
            if (NULL != child_appended)
                *child_appended += path.substr(p1 + 1);
        }
    }

    static void split(const std::wstring& path, std::wstring *parent_appended, std::wstring *child_appended)
    {
        assert(NULL != parent_appended || NULL != child_appended);

        // 找到最后一个 '/'
        std::wstring::size_type p1 = path.find_last_of(L'\\'), p2 = path.find_last_of(L'/');
        if (std::wstring::npos == p1 && std::wstring::npos == p2)
        {
            if (NULL != child_appended)
                *child_appended += path;
            return;
        }
        else if (std::wstring::npos != p1 && std::wstring::npos != p2)
        {
            p1 = (p1 > p2 ? p1 : p2);
        }
        else if (std::wstring::npos == p1)
        {
            p1 = p2;
        }

        assert(std::wstring::npos != p1);
        if (0 == p1 || L':' == path[p1 - 1]) // 磁盘号 + 根目录
        {
            if (NULL != parent_appended)
                *parent_appended += path.substr(0, p1 + 1);
            if (NULL != child_appended)
                *child_appended += path.substr(p1 + 1);
        }
        else
        {
            if (NULL != parent_appended)
                *parent_appended += path.substr(0, p1);
            if (NULL != child_appended)
                *child_appended += path.substr(p1 + 1);
        }
    }

    /**
     * 从路径中划分出磁盘号和路径(linux路径的磁盘号假定为"")
     *
     * 例如：
     * "c:\\mn\\p" -> "c:" "\\mn\\p"
     * "/mnt/sdcard" -> "" "/mnt/sdcard"
     */
    static void split_drive(const std::string& path, std::string *drive_appended, std::string *rest_appended)
    {
        assert(NULL != drive_appended || NULL != rest_appended);

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
        {
            if (NULL != rest_appended)
                *rest_appended += path;
            return;
        }
        if (NULL != drive_appended)
            *drive_appended += path.substr(0, pos + 1);
        if (NULL != rest_appended)
            *rest_appended += path.substr(pos + 1);
    }

    static void split_drive(const std::wstring& path, std::wstring *drive_appended, std::wstring *rest_appended)
    {
        assert(NULL != drive_appended || NULL != rest_appended);

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
        {
            if (NULL != rest_appended)
                *rest_appended += path;
            return;
        }
        if (NULL != drive_appended)
            *drive_appended += path.substr(0, pos + 1);
        if (NULL != rest_appended)
            *rest_appended += path.substr(pos + 1);
    }

    /**
     * 从路径或者文件名中分离出后缀名
     *
     * 例如：
     * "a.txt" -> "a" ".txt"
     */
    static void split_ext(const std::string& path, std::string *prefix_appended, std::string *ext_appended)
    {
        assert(NULL != prefix_appended || NULL != ext_appended);

        int pos = -1;
        for (int i = (int) path.length() - 1; i >= 0; --i)
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
        {
            if (NULL != prefix_appended)
                *prefix_appended += path;
            return;
        }
        if (NULL != prefix_appended)
            *prefix_appended += path.substr(0, pos);
        if (NULL != ext_appended)
            *ext_appended += path.substr(pos);
    }

    static void split_ext(const std::wstring& path, std::wstring *prefix_appended, std::wstring *ext_appended)
    {
        assert(NULL != prefix_appended || NULL != ext_appended);

        int pos = -1;
        for (int i = (int) path.length() - 1; i >= 0; --i)
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
        {
            if (NULL != prefix_appended)
                *prefix_appended += path;
            return;
        }
        if (NULL != prefix_appended)
            *prefix_appended += path.substr(0, pos);
        if (NULL != ext_appended)
            *ext_appended += path.substr(pos);
    }

    // TODO static void splitunc() {}

    /**
     * 检查路径是否存在
     */
    static bool exists(const char *path)
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

    static bool exists(const std::string& path)
    {
        return exists(path.c_str());
    }

    static bool exists(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return -1 != ::_waccess(path, 0);
#else
        std::string p;
        wstr_to_ascii(path, &p);
        return exists(p.c_str());
#endif
    }

    static bool exists(const std::wstring& path)
    {
        return exists(path.c_str());
    }

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

    static time_t getatime(const std::string& path)
    {
        return getatime(path.c_str());
    }

    static time_t getatime(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_atime;
#else
        std::string p;
        wstr_to_ascii(path, &p);
        return getatime(p.c_str());
#endif
    }

    static time_t getatime(const std::wstring& path)
    {
        return getatime(path.c_str());
    }

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

    static time_t getmtime(const std::string& path)
    {
        return getmtime(path.c_str());
    }

    static time_t getmtime(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_mtime;
#else
        std::string p;
        wstr_to_ascii(path, &p);
        return getmtime(p.c_str());
#endif
    }

    static time_t getmtime(const std::wstring& path)
    {
        return getmtime(path.c_str());
    }

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

    static time_t getctime(const std::string& path)
    {
        return getctime(path.c_str());
    }

    static time_t getctime(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_ctime;
#else
        std::string p;
        wstr_to_ascii(path, &p);
        return getctime(p.c_str());
#endif
    }

    static time_t getctime(const std::wstring& path)
    {
        return getctime(path.c_str());
    }

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

    static long getsize(const std::string& path)
    {
        return getsize(path.c_str());
    }

    static long getsize(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_size;
#else
        std::string p;
        wstr_to_ascii(path, &p);
        return getsize(p.c_str());
#endif
    }

    static long getsize(const std::wstring& path)
    {
        return getsize(path.c_str());
    }

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

    static bool isdir(const std::string& path)
    {
        return isdir(path.c_str());
    }

    static bool isdir(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 != (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(path));
#else
        std::string p;
        wstr_to_ascii(path, &p);
        return isdir(p.c_str());
#endif
    }

    static bool isdir(const std::wstring& path)
    {
        return isdir(path.c_str());
    }

    static bool isfile(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesA(path));
#else
        struct stat info;
        if (0 != ::stat(path, &info))
            return false;
        return S_ISREG(info.st_mode); // regular file
#endif
    }

    static bool isfile(const std::string& path)
    {
        return isfile(path.c_str());
    }

    static bool isfile(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(path));
#else
        std::string p;
        wstr_to_ascii(path, &p);
        return isfile(p.c_str());
#endif
    }

    static bool isfile(const std::wstring& path)
    {
        return isfile(path.c_str());
    }

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

    static bool islink(const std::string& path)
    {
        return islink(path.c_str());
    }

    static bool islink(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return false;
#else
        std::string p;
        wstr_to_ascii(path, &p);
        return isfile(p.c_str());
#endif
    }

    static bool islink(const std::wstring& path)
    {
        return islink(path.c_str());
    }

    // TODO static bool ismount() {}

    /**
     * 连接两个子路径
     *
     * 例如：
     * "a" "b" -> "a/b"
     * "/" "sd" -> "/sd"
     * "c:" "\\tmp" -> "c:\\tmp"
     */
    static void join(const std::string& a, const std::string& b, std::string *appended)
    {
        assert(NULL != appended);

        if (a.length() == 0)
        {
            *appended += b;
            return;
        }

        // 处理磁盘号
        if (':' == *a.rbegin())
        {
            *appended += a;
            *appended += b;
            return;
        }

        // 处理根目录 '/', 'c:\\'
        if (b.length() > 0 && (is_path_separator(*b.begin()) || std::string::npos != b.find_first_of(':')))
        {
            *appended += b;
            return;
        }

        // 连接
        if (is_path_separator(*a.rbegin()))
        {
            *appended += a;
            *appended += b;
        }
        else
        {
            *appended += a;
            appended->push_back(seperator());
            *appended += b;
        }
    }

    static void join(const std::wstring& a, const std::wstring& b, std::wstring *appended)
    {
        assert(NULL != appended);

        if (a.length() == 0)
        {
            *appended += b;
            return;
        }

        // 处理磁盘号
        if (L':' == *a.rbegin())
        {
            *appended += a;
            *appended += b;
            return;
        }

        // 处理根目录 '/', 'c:\\'
        if (b.length() > 0 && (is_path_separator(*b.begin()) || std::wstring::npos != b.find_first_of(L':')))
        {
            *appended += b;
            return;
        }

        // 连接
        if (is_path_separator(*a.rbegin()))
        {
            *appended += a;
            *appended += b;
        }
        else
        {
            *appended += a;
            appended->push_back(wseperator());
            *appended += b;
        }
    }

    static std::string join(const std::string& a, const std::string& b)
    {
        std::string ret;
        join(a, b, &ret);
        return ret;
    }

    static std::wstring join(const std::wstring& a, const std::wstring& b)
    {
        std::wstring ret;
        join(a, b, &ret);
        return ret;
    }

    static std::string join(const std::string& a, const std::string& b, const std::string& c)
    {
        return join(join(a, b), c);
    }

    static std::wstring join(const std::wstring& a, const std::wstring& b, const std::wstring& c)
    {
        return join(join(a, b),c);
    }

    static std::string join(const std::string& a, const std::string& b, const std::string& c, const std::string& d)
    {
        return join(join(a, b, c), d);
    }

    static std::wstring join(const std::wstring& a, const std::wstring& b, const std::wstring& c, const std::wstring& d)
    {
        return join(join(a, b, c), d);
    }
};

}

#endif
