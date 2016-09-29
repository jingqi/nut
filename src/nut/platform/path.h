
#ifndef ___HEADFILE_E6D40B10_E5D6_4092_A38B_4E69E5B8E123_
#define ___HEADFILE_E6D40B10_E5D6_4092_A38B_4E69E5B8E123_

#include <string>

#include <nut/nut_config.h>

namespace nut
{

class NUT_API Path
{
private:
    Path(); // invalid

public:
    /**
     * 路径分隔符
     */
    static char seperator();
    static wchar_t wseperator();

    /**
     * 检查字符是否是路径分隔符 '\\' '/'
     */
    static bool is_path_separator(char c);
    static bool is_path_separator(wchar_t c);

    /**
     * 获取当前工作目录路径
     */
    static void get_cwd(std::string *result);
    static void get_cwd(std::wstring *result);
    static std::string get_cwd();
    static std::wstring get_wcwd();

    /**
     * 设置当前工作路径
     */
    static void chdir(const char *cwd);
    static void chdir(const std::string& cwd);

    /**
     * 查看指定路径是否为绝对路径
     */
    static bool is_abs(const char *path);
    static bool is_abs(const wchar_t *path);
    static bool is_abs(const std::string& path);
    static bool is_abs(const std::wstring& path);

    /**
     * 将相对路径转换为绝对路径
     */
    static void abs_path(const char *path, std::string *result);
    static void abs_path(const wchar_t *path, std::wstring *result);
    static void abs_path(const std::string& path, std::string *result);
    static void abs_path(const std::wstring& path, std::wstring *result);
    static std::string abs_path(const char *path);
    static std::wstring abs_path(const wchar_t *path);
    static std::string abs_path(const std::string& path);
    static std::wstring abs_path(const std::wstring& path);

    /**
     * 将路径转换为相对路径
     */
    static void relative_path(const char *input_path, const char *ref_path,
                              std::string *result);
    static void relative_path(const wchar_t *input_path, const wchar_t *ref_path,
                              std::wstring *result);
    static void relative_path(const std::string& input_path, const std::string& ref_path,
                              std::string *result);
    static void relative_path(const std::wstring& input_path, const std::wstring& ref_path,
                              std::wstring *result);
    static std::string relative_path(const char *input_path, const char *ref_path);
    static std::wstring relative_path(const wchar_t *input_path, const wchar_t *ref_path);
    static std::string relative_path(const std::string& input_path, const std::string& ref_path);
    static std::wstring relative_path(const std::wstring& input_path, const std::wstring& ref_path);

    // TODO static std::string real_path() {}

    /**
     * 从路径中划分出父路径和 文件/文件夹 名
     *
     * 例如：
     * "ab/c/d" -> "ab/c" "d"
     * "/ab.txt" -> "/" "ab.txt"
     * "c:\\tmp" -> "c:\\" "tmp"
     */
    static void split(const char *path, std::string *parent_result, std::string *child_result);
    static void split(const wchar_t *path, std::wstring *parent_result, std::wstring *child_result);
    static void split(const std::string& path, std::string *parent_result, std::string *child_result);
    static void split(const std::wstring& path, std::wstring *parent_result, std::wstring *child_result);

    /**
     * 从路径中划分出磁盘号和路径(linux路径的磁盘号假定为"")
     *
     * 例如：
     * "c:\\mn\\p" -> "c:" "\\mn\\p"
     * "/mnt/sdcard" -> "" "/mnt/sdcard"
     */
    static void split_drive(const char *path, std::string *drive_result, std::string *rest_result);
    static void split_drive(const wchar_t *path, std::wstring *drive_result, std::wstring *rest_result);
    static void split_drive(const std::string& path, std::string *drive_result, std::string *rest_result);
    static void split_drive(const std::wstring& path, std::wstring *drive_result, std::wstring *rest_result);

    /**
     * 从路径或者文件名中分离出后缀名
     *
     * 例如：
     * "a.txt" -> "a" ".txt"
     */
    static void split_ext(const char *path, std::string *prefix_result, std::string *ext_result);
    static void split_ext(const wchar_t *path, std::wstring *prefix_result, std::wstring *ext_result);
    static void split_ext(const std::string& path, std::string *prefix_result, std::string *ext_result);
    static void split_ext(const std::wstring& path, std::wstring *prefix_result, std::wstring *ext_result);

    // TODO static void splitunc() {}

    /**
     * 检查路径存在性(不解析符号链接)
     */
    static bool lexists(const char *path);
    static bool lexists(const wchar_t *path);
    static bool lexists(const std::string& path);
    static bool lexists(const std::wstring& path);

    /**
     * 检查路径存在性(解析符号链接)
     */
    static bool exists(const char *path);
    static bool exists(const wchar_t *path);
    static bool exists(const std::string& path);
    static bool exists(const std::wstring& path);

    /**
     * last access time
     */
    static time_t get_atime(const char *path);
    static time_t get_atime(const wchar_t *path);
    static time_t get_atime(const std::string& path);
    static time_t get_atime(const std::wstring& path);

    /**
     * last modified time
     */
    static time_t get_mtime(const char *path);
    static time_t get_mtime(const wchar_t *path);
    static time_t get_mtime(const std::string& path);
    static time_t get_mtime(const std::wstring& path);

    /**
     * created time
     */
    static time_t get_ctime(const char *path);
    static time_t get_ctime(const wchar_t *path);
    static time_t get_ctime(const std::string& path);
    static time_t get_ctime(const std::wstring& path);

    /**
     * 获取文件大小
     */
    static long long get_size(const char *path);
    static long long get_size(const wchar_t *path);
    static long long get_size(const std::string& path);
    static long long get_size(const std::wstring& path);

    static bool is_dir(const char *path);
    static bool is_dir(const wchar_t *path);
    static bool is_dir(const std::string& path);
    static bool is_dir(const std::wstring& path);

    static bool is_file(const char *path);
    static bool is_file(const wchar_t *path);
    static bool is_file(const std::string& path);
    static bool is_file(const std::wstring& path);

    static bool is_link(const char *path);
    static bool is_link(const wchar_t *path);
    static bool is_link(const std::string& path);
    static bool is_link(const std::wstring& path);

    // TODO static bool ismount() {}

    /**
     * 连接两个子路径
     *
     * 例如：
     * "a" "b" -> "a/b"
     * "/" "sd" -> "/sd"
     * "c:" "\\tmp" -> "c:\\tmp"
     */
    static void join(const char *a, const char *b, std::string *result);
    static void join(const wchar_t *a, const wchar_t *b, std::wstring *result);
    static void join(const std::string& a, const std::string& b, std::string *result);
    static void join(const std::wstring& a, const std::wstring& b, std::wstring *result);
    static std::string join(const char *a, const char *b);
    static std::wstring join(const wchar_t *a, const wchar_t *b);
    static std::string join(const std::string& a, const std::string& b);
    static std::wstring join(const std::wstring& a, const std::wstring& b);

    static void join(const char *a, const char *b, const char *c, std::string *result);
    static void join(const wchar_t *a, const wchar_t *b, const wchar_t *c, std::wstring *result);
    static void join(const std::string& a, const std::string& b, const std::string& c, std::string *result);
    static void join(const std::wstring& a, const std::wstring& b, const std::wstring& c, std::wstring *result);
    static std::string join(const char *a, const char *b, const char *c);
    static std::wstring join(const wchar_t *a, const wchar_t *b, const wchar_t *c);
    static std::string join(const std::string& a, const std::string& b, const std::string& c);
    static std::wstring join(const std::wstring& a, const std::wstring& b, const std::wstring& c);

    static void join(const char *a, const char *b, const char *c, const char *d, std::string *result);
    static void join(const wchar_t *a, const wchar_t *b, const wchar_t *c, const wchar_t *d, std::wstring *result);
    static void join(const std::string& a, const std::string& b, const std::string& c, const std::string& d, std::string *result);
    static void join(const std::wstring& a, const std::wstring& b, const std::wstring& c, const std::wstring& d, std::wstring *result);
    static std::string join(const char *a, const char *b, const char *c, const char *d);
    static std::wstring join(const wchar_t *a, const wchar_t *b, const wchar_t *c, const wchar_t *d);
    static std::string join(const std::string& a, const std::string& b, const std::string& c, const std::string& d);
    static std::wstring join(const std::wstring& a, const std::wstring& b, const std::wstring& c, const std::wstring& d);
};

}

#endif
