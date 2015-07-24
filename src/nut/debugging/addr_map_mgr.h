
#ifndef ___HEADFILE_95CF1F75_72F1_454A_AA4C_B0D8DD34F14A_
#define ___HEADFILE_95CF1F75_72F1_454A_AA4C_B0D8DD34F14A_

#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_OS_LINUX)

#include <assert.h>
#include <map>
#include <string>
#include <cstring>
#include <unistd.h>
#include <execinfo.h> // for backtrace() and backtrace_symbols()
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <limits.h> // for PATH_MAX
#include <fstream>
#include <stdint.h>

#include <nut/util/string/string_util.h>

namespace nut
{

/**
 * 负责记录动态链接库在进程空间的加载地址
 */
class AddrMapManager
{
public:
    typedef unsigned int addr_t;

private:
    // 析构检查字段
    uint32_t _destruct_tag;

    /**
     * key 为动态链接库实际绝对路径（不是软链接，也不是相对路径）
     * value为动态链接库加载地址
     */
    typedef std::map<std::string, addr_t> addr_map_t;
    addr_map_t _addr_map;

    // 程序的绝对路径
    std::string _exec_path;

    /**
     * 文件"/proc/xxx/maps"的路径，其中xxx为进程ID.
     * 该文件保存着共享库在进程空间的映射地址.
     */
    std::string _maps_path;

public:
    AddrMapManager();
    ~AddrMapManager();

    /**
     * 检查结构体是否依然有效
     */
    bool is_valid() const;

    /**
     * 获取应用程序绝对路径
     */
    const std::string& get_exec_path() const
    {
        return _exec_path;
    }

	/**
     * 从文件 _maps_path 中加载地址的映射，path为空表示加载所有。
     */
    void load(const std::string& path = std::string());

    /**
     * 找到某个共享库的起始地址
     */
    bool find(const std::string& path, addr_t *out_addr);

private:
    /**
     * 本函数用于获取从文件"/proc/进程ID/maps"中获取的一行的信息，形如：
     * 00577000-00578000 r-xp 00000000 08:01 160313     /usr/local/lib/libbtshare2.so
     * 它表示共享库在程序中的加载地址
     */
    bool parse_line(const std::string& str_line, bool fappoint_path = false, const std::string& path = std::string());
};

}

#endif

#endif
