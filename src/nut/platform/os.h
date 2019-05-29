
#ifndef ___HEADFILE_291DFB4C_7D29_4D61_A691_EF83FB86CD36_
#define ___HEADFILE_291DFB4C_7D29_4D61_A691_EF83FB86CD36_

#include <string>
#include <vector>

#include "../nut_config.h"


namespace nut
{

class NUT_API OS
{
public:
    /**
     * 枚举目录下的文件/文件夹
     *
     * @param exclude_file 如果传入true, 则返回值不会包含文件
     * @parma exclude_dir 如果传入true, 则返回值不会包含文件夹
     * @parma exclude_initial_dot 如果传入true, 则返回值不会包含以'.'开头的文件/文件夹
     */
    static std::vector<std::string> listdir(
        const std::string& path, bool exclude_file = false,
        bool exclude_dir = false, bool exclude_initial_dot = false);

    static std::vector<std::wstring> listdir(
        const std::wstring& path, bool exclude_file = false,
        bool exclude_dir = false, bool exclude_initial_dot = false);

    /**
     * 复制文件
     */
    static bool copyfile(const std::string& src, const std::string& dst);
    static bool copyfile(const std::wstring& src, const std::wstring& dst);

    /**
     * 删除文件
     */
    static bool removefile(const std::string& path);
    static bool removefile(const std::wstring& path);

    /**
     * 创建目录
     */
    static bool mkdir(const std::string& path);
    static bool mkdir(const std::wstring& path);

    /**
     * 创建目录树
     */
    static bool mkdirs(const std::string& path);
    static bool mkdirs(const std::wstring& path);

    /**
     * 删除空目录
     */
    static bool rmdir(const std::string& path);
    static bool rmdir(const std::wstring& path);

    /**
     * 删除目录树
     */
    static bool rmtree(const std::string& path);
    static bool rmtree(const std::wstring& path);

    /**
     * 读取符号链接地址
     */
    static std::string readlink(const std::string& path);
    static std::wstring readlink(const std::wstring& path);

    /**
     * 创建符号链接
     *
     * @param link 软链接指向的位置
     * @param path 软链接本身所在的位置
     */
    static bool symlink(const std::string& link, const std::string& path);
    static bool symlink(const std::wstring& link, const std::wstring& path);

    /**
     * 重命名/移动
     */
    static bool rename(const std::string& from, const std::string& to);
    static bool rename(const std::wstring& from, const std::wstring& to);

private:
    OS() = delete;
};

}

#endif
