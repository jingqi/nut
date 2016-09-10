
#ifndef ___HEADFILE_291DFB4C_7D29_4D61_A691_EF83FB86CD36_
#define ___HEADFILE_291DFB4C_7D29_4D61_A691_EF83FB86CD36_

#include <string>
#include <vector>

#include <nut/nut_config.h>

namespace nut
{

class NUT_API OS
{
private:
    OS(); // invalid

public:
    /**
     * 枚举目录下的文件/文件夹
     *
     * @param exclude_file 如果传入true, 则返回值不会包含文件
     * @parma exclude_dir 如果传入true, 则返回值不会包含文件夹
     * @parma exclude_initial_dot 如果传入true, 则返回值不会包含以'.'开头的文件/文件夹
     */
    static void list_dir(const char *path, std::vector<std::string> *result,
                         bool exclude_file = false, bool exclude_dir = false,
                         bool exclude_initial_dot = false);
    static void list_dir(const wchar_t *path, std::vector<std::wstring> *result,
                         bool exclude_file = false, bool exclude_dir = false,
                         bool exclude_initial_dot = false);
    static void list_dir(const std::string& path, std::vector<std::string> *result,
                         bool exclude_file = false, bool exclude_dir = false,
                         bool exclude_initial_dot = false);
    static void list_dir(const std::wstring& path, std::vector<std::wstring> *result,
                         bool exclude_file = false, bool exclude_dir = false,
                         bool exclude_initial_dot = false);

    /**
     * 复制文件
     */
    static bool copy_file(const char *src, const char *dst);
    static bool copy_file(const wchar_t *src, const wchar_t *dst);
    static bool copy_file(const std::string& src, const std::string& dst);
    static bool copy_file(const std::wstring& src, const std::wstring& dst);

    /**
     * 删除文件
     */
    static bool remove_file(const char *path);
    static bool remove_file(const wchar_t *path);
    static bool remove_file(const std::string& path);
    static bool remove_file(const std::wstring& path);

    /**
     * 创建目录
     */
    static bool mkdir(const char *path);
    static bool mkdir(const wchar_t *path);
    static bool mkdir(const std::string& path);
    static bool mkdir(const std::wstring& path);

    /**
     * 创建目录树
     */
    static bool mkdirs(const char *path);
    static bool mkdirs(const wchar_t *path);
    static bool mkdirs(const std::string& path);
    static bool mkdirs(const std::wstring& path);

    /**
     * 删除空目录
     */
    static bool rmdir(const char *path);
    static bool rmdir(const wchar_t *path);
    static bool rmdir(const std::string& path);
    static bool rmdir(const std::wstring& path);

    /**
     * 删除目录树
     */
    static bool remove_tree(const char *path);
    static bool remove_tree(const wchar_t *path);
    static bool remove_tree(const std::string& path);
    static bool remove_tree(const std::wstring& path);

    /**
     * 读取符号链接地址
     */
    static bool read_link(const char *path, std::string *result);
    static bool read_link(const wchar_t *path, std::wstring *result);
    static bool read_link(const std::string& path, std::string *result);
    static bool read_link(const std::wstring& path, std::wstring *result);

    /**
     * 创建符号链接
     *
     * @param link 软链接指向的位置
     * @param path 软链接本身所在的位置
     */
    static bool symlink(const char *link, const char *path);
    static bool symlink(const wchar_t *link, const wchar_t *path);
    static bool symlink(const std::string& link, const std::string& path);
    static bool symlink(const std::wstring& link, const std::wstring& path);

    /**
     * 重命名
     */
    static bool rename(const char *from, const char *to);
    static bool rename(const wchar_t *from, const wchar_t *to);
    static bool rename(const std::string& from, const std::string& to);
    static bool rename(const std::wstring& from, const std::wstring& to);
};

}

#endif
