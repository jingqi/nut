
#ifndef ___HEADFILE_E6D40B10_E5D6_4092_A38B_4E69E5B8E123_
#define ___HEADFILE_E6D40B10_E5D6_4092_A38B_4E69E5B8E123_

#include <string>
#include <vector>

#include "../nut_config.h"
#include "platform.h"


namespace nut
{

class NUT_API Path
{
public:
    /**
     * 路径分隔符
     */
    constexpr static char sep()
    {
#if NUT_PLATFORM_OS_WINDOWS
        return '\\';
#else
        return '/';
#endif
    }

    constexpr static wchar_t wsep()
    {
#if NUT_PLATFORM_OS_WINDOWS
        return L'\\';
#else
        return L'/';
#endif
    }

    /**
     * 检查字符是否是路径分隔符 '\' '/'
     */
    constexpr static bool is_sep(char c)
    {
        return '\\' == c || '/' == c;
    }

    constexpr static bool is_sep(wchar_t c)
    {
        return L'\\' == c || L'/' == c;
    }

    /**
     * 获取当前工作目录路径
     */
    static std::string get_cwd();
    static std::wstring get_wcwd();

    /**
     * 设置当前工作路径
     */
    static void chdir(const std::string& cwd);
    static void chdir(const std::wstring& cwd);

    /**
     * 是否为根目录
     *
     * 例如:
     *   "c:"      (only for windows)
     *   "c:\"     (only for windows)
     *   "c:/"     (only for windows)
     *   "/"
     *   "\"
     */
    static bool is_root(const std::string& path);
    static bool is_root(const std::wstring& path);

    /**
     * 查看指定路径是否为绝对路径
     */
    static bool is_abs(const std::string& path);
    static bool is_abs(const std::wstring& path);

    /**
     * 将相对路径转换为绝对路径
     */
    static std::string abspath(const std::string& path);
    static std::wstring abspath(const std::wstring& path);

    /**
     * 解析软连接, 并转换为绝对路径
     */
    static std::string realpath(const std::string& path);
    static std::wstring realpath(const std::wstring& path);

    /**
     * 将路径转换为相对路径
     */
    static std::string relpath(const std::string& input_path, const std::string& ref_path);
    static std::wstring relpath(const std::wstring& input_path, const std::wstring& ref_path);

    /**
     * 例如:
     *   "a/b/c" -> "a/b"
     *   "a/b/c/" -> "a/b"
     */
    static std::string dirname(const std::string& path);
    static std::wstring dirname(const std::wstring& path);

    /**
     * 例如:
     *   "a/b/c" -> "c"
     *   "a/b/c/" -> "c"
     */
    static std::string basename(const std::string& path);
    static std::wstring basename(const std::wstring& path);

    /**
     * 从路径中划分出 dirname 和 basename
     *
     * 例如:
     *   "ab/c/d" -> "ab/c" "d"
     *   "/ab.txt" -> "/" "ab.txt"
     *   "c:\tmp" -> "c:\" "tmp"
     */
    static void split(const std::string& path, std::string *dirname, std::string *basename);
    static void split(const std::wstring& path, std::wstring *dirname, std::wstring *basename);

    /**
     * 拆分出路径上所有 目录/文件 名
     *
     * 例如:
     *   "a/b/c" -> "a" "b" "c"
     *   "c:/a/../b" -> "c:/" "a" ".." "b"
     *   "/m/n" -> "/" "m" "n"
     */
    static std::vector<std::string> split_entries(const std::string& path);
    static std::vector<std::wstring> split_entries(const std::wstring& path);

    /**
     * 从路径中划分出磁盘号和路径(linux路径的磁盘号假定为"")
     *
     * 例如:
     *   "c:\mn\p" -> "c:" "\mn\p"
     *   "/mnt/sdcard" -> "" "/mnt/sdcard"
     */
    static void split_drive(const std::string& path, std::string *drive, std::string *rest);
    static void split_drive(const std::wstring& path, std::wstring *drive, std::wstring *rest);

    /**
     * 从路径或者文件名中分离出后缀名
     *
     * 例如:
     *   "a.txt" -> "a" ".txt"
     */
    static void split_ext(const std::string& path, std::string *prefix, std::string *ext);
    static void split_ext(const std::wstring& path, std::wstring *prefix, std::wstring *ext);

    /**
     * 连接多个子路径
     *
     * 例如:
     *   "a" "b" -> "a/b"
     *   "/" "sd" -> "/sd"
     *   "c:" "\tmp" -> "c:\tmp"
     */
    static std::string join(const std::string& a, const std::string& b);
    static std::wstring join(const std::wstring& a, const std::wstring& b);

    static std::string join(const std::string& a, const std::string& b, const std::string& c);
    static std::wstring join(const std::wstring& a, const std::wstring& b, const std::wstring& c);

    static std::string join(const std::string& a, const std::string& b, const std::string& c,
                            const std::string& d);
    static std::wstring join(const std::wstring& a, const std::wstring& b, const std::wstring& c,
                             const std::wstring& d);

    static std::string join(const std::string& a, const std::string& b, const std::string& c,
                            const std::string& d, const std::string& e);
    static std::wstring join(const std::wstring& a, const std::wstring& b, const std::wstring& c,
                             const std::wstring& d, const std::wstring& e);

    /**
     * 检查路径存在性
     */
    static bool exists(const std::string& path);
    static bool exists(const std::wstring& path);

    /**
     * 检查路径存在性(不解析符号链接)
     */
    static bool lexists(const std::string& path);
    static bool lexists(const std::wstring& path);

    /**
     * 获取最后访问时间
     */
    static time_t get_atime(const std::string& path);
    static time_t get_atime(const std::wstring& path);

    /**
     * 获取最后访问时间(不解析符号链接)
     */
    static time_t get_latime(const std::string& path);
    static time_t get_latime(const std::wstring& path);

    /**
     * 获取最后修改时间
     */
    static time_t get_mtime(const std::string& path);
    static time_t get_mtime(const std::wstring& path);

    /**
     * 获取最后修改时间(不解析符号链接)
     */
    static time_t get_lmtime(const std::string& path);
    static time_t get_lmtime(const std::wstring& path);

    /**
     * 获取创建时间
     */
    static time_t get_ctime(const std::string& path);
    static time_t get_ctime(const std::wstring& path);

    /**
     * 获取创建时间(不解析符号链接)
     */
    static time_t get_lctime(const std::string& path);
    static time_t get_lctime(const std::wstring& path);

    /**
     * 获取文件大小
     */
    static long long get_size(const std::string& path);
    static long long get_size(const std::wstring& path);

    static bool is_link(const std::string& path);
    static bool is_link(const std::wstring& path);

    static bool is_dir(const std::string& path);
    static bool is_dir(const std::wstring& path);

    static bool is_file(const std::string& path);
    static bool is_file(const std::wstring& path);

private:
    Path() = delete;
};

}

#endif
