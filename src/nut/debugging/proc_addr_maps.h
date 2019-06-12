
#ifndef ___HEADFILE_95CF1F75_72F1_454A_AA4C_B0D8DD34F14A_
#define ___HEADFILE_95CF1F75_72F1_454A_AA4C_B0D8DD34F14A_

#include "../platform/platform.h"

#if NUT_PLATFORM_OS_LINUX

#include <stdint.h>
#include <string>
#include <unordered_map>

#include "../nut_config.h"


namespace nut
{

/**
 * 解析 /proc/xxx/maps (xxx表示进程号)，记录进程的虚拟地址空间使用情况
 */
class NUT_API ProcAddrMaps
{
public:
    typedef typename std::conditional<
        sizeof(void*) == sizeof(uint8_t), uint8_t,
        typename std::conditional<
            sizeof(void*) == sizeof(uint16_t), uint16_t,
            typename std::conditional<
                sizeof(void*) == sizeof(uint32_t), uint32_t,
                uint64_t>::type>::type>::type addr_type;

public:
    static ProcAddrMaps& instance() noexcept;

    /**
     * 检查结构体是否依然有效
     */
    bool is_valid() const noexcept;

    /**
     * 获取应用程序绝对路径
     */
    const std::string& get_exec_path() const noexcept;

    /**
     * 从文件 /proc/xxx/maps 中加载地址的映射
     *
     * @param module_path 只关心指定模块
     */
    void load(const std::string& module_path = std::string()) noexcept;

    /**
     * 找到某个共享库的起始地址
     */
    bool find(const std::string& module_path, addr_type *out_addr) const noexcept;

private:
    ProcAddrMaps() noexcept;
    ~ProcAddrMaps() noexcept;

    /**
     * 解析从文件 "/proc/xxx/maps" 中获取的一行的信息，它表示模块在程序虚拟地址
     * 空间中的映射地址，形如：
     *
     * 00577000-00578000 r-xp 00000000 08:01 160313     /usr/local/lib/libbtshare2.so
     *
     * 表示：
     * 起始虚拟地址-终止虚拟地址 权限 文件偏移 主次设备号 设备节点号 文件地址
     *
     * @param module_path 只分析指定模块
     */
    bool parse_line(const std::string& line,
                    const std::string& module_path = std::string()) noexcept;

private:
    // 析构检查字段
    uint32_t _destruct_tag = 0;

    /**
     * key 为动态链接库实际绝对路径（不是软链接，也不是相对路径）
     * value 为动态链接库加载地址
     */
    typedef std::unordered_map<std::string, addr_type> addr_map_type;
    addr_map_type _addr_map;

    // 程序的绝对路径
    std::string _exec_path;

    // 路径 "/proc/xxx/maps"
    std::string _maps_path;
};

}

#endif /* NUT_PLATFORM_OS_LINUX */

#endif /* header file guarder */
