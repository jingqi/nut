/**
 * @file -
 * @author jingqi
 * @date 2012-06-23
 * @last-edit 2014-11-21 23:05:39 jingqi
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
#   include <limits.h> // for PATH_MAX
#endif

#include <nut/container/tuple.hpp>
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

    static inline void getCwd(std::string *out)
    {
        assert(NULL != out);

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

        *out = buf;
    }

    static inline void getCwd(std::wstring *out)
    {
        assert(NULL != out);

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

        ascii_to_wstr(buf, out);
    }

    static inline std::string getCwd()
    {
        std::string ret;
        getCwd(&ret);
        return ret;
    }

    static inline std::wstring getWCwd()
    {
        std::wstring ret;
        getCwd(&ret);
        return ret;
    }

    static inline bool isabs(const std::string& p)
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
            else if (isPathSeparator(c))
                return false;
        }
        return false;
    }

    static inline bool isabs(const std::wstring& p)
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
            else if (isPathSeparator(c))
                return false;
        }
        return false;
    }

    static inline void abspath(const std::string& p, std::string *out)
    {
        assert(NULL != out);
        if (p.empty())
        {
            getCwd(out);
            return;
        }

        // 探测是否从根路径开始
        out->clear();
        if (!isabs(p))
            getCwd(out);

        // 组装路径
        std::string part;
        for (size_t i = 0, len = p.length(); i < len; ++i)
        {
            const char c = p.at(i);
            if (!isPathSeparator(c))
            {
                part.push_back(c);
                if (i != len - 1)
                    continue;
            }

            // 处理根目录
            if (out->empty())
            {
                if (part.empty())
                {
                    // linux root
                    out->push_back('/');
                    continue;
                }
                else if (part.at(part.length() - 1) == ':')
                {
                    // windows partition root
                    *out += part;
                    out->push_back('\\');
                    part.clear();
                    continue;
                }
            }

            // 组装
            if (!part.empty())
            {
                if (part == "..")
                {
                    int j = (int) out->size() - 1;
                    if (isPathSeparator(out->at(j)))
                        --j;
                    while (j >= 0)
                    {
                        if (isPathSeparator(out->at(j)))
                            break;
                        --j;
                    }
                    if (j == 0 || (j > 0 && out->at(j - 1) == ':'))
                        out->resize(j + 1);
                    else if(j > 0)
                        out->resize(j);
                    // else: parent directory out of range
                }
                else if (part != ".")
                {
                    if (!out->empty() && !isPathSeparator(out->at(out->length() - 1)))
                    {
                        out->push_back(seperator());
                    }
                    *out += part;
                }
                part.clear();
            }
        }
    }

    static inline void abspath(const std::wstring& p, std::wstring *out)
    {
        assert(NULL != out);
        if (p.empty())
        {
            getCwd(out);
            return;
        }

        // 探测是否从根路径开始
        out->clear();
        if (!isabs(p))
            getCwd(out);

        // 组装路径
        std::wstring part;
        for (size_t i = 0, len = p.length(); i < len; ++i)
        {
            const wchar_t c = p.at(i);
            if (!isPathSeparator(c))
            {
                part.push_back(c);
                if (i != len - 1)
                    continue;
            }

            // 处理根目录
            if (out->empty())
            {
                if (part.empty())
                {
                    // linux root
                    out->push_back(L'/');
                    continue;
                }
                else if (part.at(part.length() - 1) == L':')
                {
                    // windows partition root
                    *out += part;
                    out->push_back(L'\\');
                    part.clear();
                    continue;
                }
            }

            // 组装
            if (!part.empty())
            {
                if (part == L"..")
                {
                    int j = (int) out->size() - 1;
                    if (isPathSeparator(out->at(j)))
                        --j;
                    while (j >= 0)
                    {
                        if (isPathSeparator(out->at(j)))
                            break;
                        --j;
                    }
                    if (j == 0 || (j > 0 && out->at(j - 1) == L':'))
                        out->resize(j + 1);
                    else if(j > 0)
                        out->resize(j);
                    // else: parent directory out of range
                }
                else if (part != L".")
                {
                    if (!out->empty() && !isPathSeparator(out->at(out->length() - 1)))
                    {
                        out->push_back(seperator());
                    }
                    *out += part;
                }
                part.clear();
            }
        }
    }

    static inline std::string abspath(const std::string& p)
    {
        std::string ret;
        abspath(p, &ret);
        return ret;
    }

    static inline std::wstring abspath(const std::wstring& p)
    {
        std::wstring ret;
        abspath(p, &ret);
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
    static inline void split(const std::string &path, std::string *parent, std::string *child)
    {
        assert(NULL != parent || NULL != child);

        // 找到最后一个 '/'
        std::string::size_type p1 = path.find_last_of('\\'), p2 = path.find_last_of('/');
        if (std::string::npos == p1 && std::string::npos == p2)
        {
            if (NULL != parent)
                parent->clear();
            if (NULL != child)
                *child = path;
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
            if (NULL != parent)
                *parent = path.substr(0, p1 + 1);
            if (NULL != child)
                *child = path.substr(p1 + 1);
        }
        else
        {
            if (NULL != parent)
                *parent = path.substr(0, p1);
            if (NULL != child)
                *child = path.substr(p1 + 1);
        }
    }

    static inline void split(const std::wstring& path, std::wstring *parent, std::wstring *child)
    {
        assert(NULL != parent || NULL != child);

        // 找到最后一个 '/'
        std::wstring::size_type p1 = path.find_last_of(L'\\'), p2 = path.find_last_of(L'/');
        if (std::wstring::npos == p1 && std::wstring::npos == p2)
        {
            if (NULL != parent)
                parent->clear();
            if (NULL != child)
                *child = path;
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
            if (NULL != parent)
                *parent = path.substr(0, p1 + 1);
            if (NULL != child)
                *child = path.substr(p1 + 1);
        }
        else
        {
            if (NULL != parent)
                *parent = path.substr(0, p1);
            if (NULL != child)
                *child = path.substr(p1 + 1);
        }
    }

    static inline Tuple<std::string,std::string> split(const std::string& path)
    {
        Tuple<std::string,std::string> ret;
        split(path, &ret.first, &ret.second);
        return ret;
    }

    static inline Tuple<std::wstring,std::wstring> split(const std::wstring& path)
    {
        Tuple<std::wstring,std::wstring> ret;
        split(path, &ret.first, &ret.second);
        return ret;
    }

    /**
     * 从路径中划分出磁盘号和路径(linux路径的磁盘号假定为"")
     *
     * 例如：
     * "c:\\mn\\p" -> "c:" "\\mn\\p"
     * "/mnt/sdcard" -> "" "/mnt/sdcard"
     */
    static inline void splitdrive(const std::string& path, std::string *drive, std::string *rest)
    {
        assert(NULL != drive || NULL != rest);

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
            if (NULL != drive)
                drive->clear();
            if (NULL != rest)
                *rest = path;
            return;
        }
        if (NULL != drive)
            *drive = path.substr(0, pos + 1);
        if (NULL != rest)
            *rest = path.substr(pos + 1);
    }

    static inline void splitdrive(const std::wstring& path, std::wstring *drive, std::wstring *rest)
    {
        assert(NULL != drive || NULL != rest);

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
            if (NULL != drive)
                drive->clear();
            if (NULL != rest)
                *rest = path;
            return;
        }
        if (NULL != drive)
            *drive = path.substr(0, pos + 1);
        if (NULL != rest)
            *rest = path.substr(pos + 1);
    }

    static inline Tuple<std::string,std::string> splitdrive(const std::string& path)
    {
        Tuple<std::string,std::string> ret;
        splitdrive(path, &ret.first, &ret.second);
        return ret;
    }

    static inline Tuple<std::wstring,std::wstring> splitdrive(const std::wstring& path)
    {
        Tuple<std::wstring,std::wstring> ret;
        splitdrive(path, &ret.first, &ret.second);
        return ret;
    }

    /**
     * 从路径或者文件名中分离出后缀名
     *
     * 例如：
     * "a.txt" -> "a" ".txt"
     */
    static inline void splitext(const std::string& path, std::string *prefix, std::string *ext)
    {
        assert(NULL != prefix || NULL != ext);

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
            if (NULL != prefix)
                *prefix = path;
            if (NULL != ext)
                ext->clear();
            return;
        }
        if (NULL != prefix)
            *prefix = path.substr(0, pos);
        if (NULL != ext)
            *ext = path.substr(pos);
    }

    static inline void splitext(const std::wstring& path, std::wstring *prefix, std::wstring *ext)
    {
        assert(NULL != prefix || NULL != ext);

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
            if (NULL != prefix)
                *prefix = path;
            if (NULL != ext)
                ext->clear();
            return;
        }
        if (NULL != prefix)
            *prefix = path.substr(0, pos);
        if (NULL != ext)
            *ext = path.substr(pos);
    }

    static inline Tuple<std::string,std::string> splitext(const std::string& path)
    {
        Tuple<std::string,std::string> ret;
        splitext(path, &ret.first, &ret.second);
        return ret;
    }

    static inline Tuple<std::wstring,std::wstring> splitext(const std::wstring& path)
    {
        Tuple<std::wstring,std::wstring> ret;
        splitext(path, &ret.first, &ret.second);
        return ret;
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

    static inline bool exists(const std::string& path)
    {
        return exists(path.c_str());
    }

    static inline bool exists(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return -1 != ::_waccess(path, 0);
#else
        const std::string p = wstr_to_ascii(path);
        return exists(p.c_str());
#endif
    }

    static inline bool exists(const std::wstring& path) { return exists(path.c_str()); }

    /**
     * last access time
     */
    static inline time_t getatime(const char *path)
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

    static inline time_t getatime(const std::string& path)
    {
        return getatime(path.c_str());
    }

    static inline time_t getatime(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_atime;
#else
        const std::string p = wstr_to_ascii(path);
        return getatime(p.c_str());
#endif
    }

    static inline time_t getatime(const std::wstring& path)
    {
        return getatime(path.c_str());
    }


    /**
     * last modified time
     */
    static inline time_t getmtime(const char *path)
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

    static inline time_t getmtime(const std::string& path)
    {
        return getmtime(path.c_str());
    }

    static inline time_t getmtime(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_mtime;
#else
        const std::string p = wstr_to_ascii(path);
        return getmtime(p.c_str());
#endif
    }

    static inline time_t getmtime(const std::wstring& path)
    {
        return getmtime(path.c_str());
    }

    /**
     * created time
     */
    static inline time_t getctime(const char *path)
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

    static inline time_t getctime(const std::string& path)
    {
        return getctime(path.c_str());
    }

    static inline time_t getctime(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_ctime;
#else
        const std::string p = wstr_to_ascii(path);
        return getctime(p.c_str());
#endif
    }

    static inline time_t getctime(const std::wstring& path)
    {
        return getctime(path.c_str());
    }

    /**
     * 获取文件大小
     */
    static inline long getsize(const char *path)
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

    static inline long getsize(const std::string& path)
    {
        return getsize(path.c_str());
    }

    static inline long getsize(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        struct _stat info;
        ::_wstat(path, &info);
        return info.st_size;
#else
        const std::string p = wstr_to_ascii(path);
        return getsize(p.c_str());
#endif
    }

    static inline long getsize(const std::wstring& path)
    {
        return getsize(path.c_str());
    }

    static inline bool isdir(const char *path)
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

    static inline bool isdir(const std::string& path)
    {
        return isdir(path.c_str());
    }

    static inline bool isdir(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 != (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(path));
#else
        const std::string p = wstr_to_ascii(path);
        return isdir(p.c_str());
#endif
    }

    static inline bool isdir(const std::wstring& path)
    {
        return isdir(path.c_str());
    }

    static inline bool isfile(const char *path)
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

    static inline bool isfile(const std::string& path)
    {
        return isfile(path.c_str());
    }

    static inline bool isfile(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(path));
#else
        const std::string p = wstr_to_ascii(path);
        return isfile(p.c_str());
#endif
    }

    static inline bool isfile(const std::wstring& path)
    {
        return isfile(path.c_str());
    }

    static inline bool islink(const char *path)
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

    static inline bool islink(const std::string& path)
    {
        return islink(path.c_str());
    }

    static inline bool islink(const wchar_t *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return false;
#else
        const std::string p = wstr_to_ascii(path);
        return isfile(p.c_str());
#endif
    }

    static inline bool islink(const std::wstring& path)
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
    static inline void join(const std::string& a, const std::string& b, std::string *out)
    {
        assert(NULL != out);

        if (a.length() == 0)
        {
            *out = b;
            return;
        }

        // 处理磁盘号
        if (':' == *a.rbegin())
        {
            *out = a + b;
            return;
        }

        // 处理根目录 '/', 'c:\\'
        if (b.length() > 0 && (isPathSeparator(*b.begin()) || std::string::npos != b.find_first_of(':')))
        {
            *out = b;
            return;
        }

        // 连接
        if (isPathSeparator(*a.rbegin()))
            *out = a + b;
        else
            *out = a + seperator() + b;
    }

    static inline void join(const std::wstring& a, const std::wstring& b, std::wstring *out)
    {
        assert(NULL != out);

        if (a.length() == 0)
        {
            *out = b;
            return;
        }

        // 处理磁盘号
        if (L':' == *a.rbegin())
        {
            *out = a + b;
            return;
        }

        // 处理根目录 '/', 'c:\\'
        if (b.length() > 0 && (isPathSeparator(*b.begin()) || std::wstring::npos != b.find_first_of(L':')))
        {
            *out = b;
            return;
        }

        // 连接
        if (isPathSeparator(*a.rbegin()))
            *out = a + b;
        else
            *out = a + wseperator() + b;
    }

    static inline std::string join(const std::string& a, const std::string& b)
    {
        std::string ret;
        join(a, b, &ret);
        return ret;
    }

    static inline std::wstring join(const std::wstring& a, const std::wstring& b)
    {
        std::wstring ret;
        join(a, b, &ret);
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
