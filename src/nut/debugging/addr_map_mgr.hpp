/**
 * @file -
 * @author jingqi
 * @date 2013-08-30
 * @last-edit 2015-01-06 21:52:06 jingqi
 * @brief
 */

#ifndef ___HEADFILE_95CF1F75_72F1_454A_AA4C_B0D8DD34F14A_
#define ___HEADFILE_95CF1F75_72F1_454A_AA4C_B0D8DD34F14A_

#include <nut/platform/platform.hpp>

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

#include <nut/util/string/string_util.hpp>

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
	enum
	{
		CONSTRUCTED_TAG = 0xA5A55A5A, // 构造后的 tag
		DESTRUCTED_TAG = 0x0 // 析构后的 tag
	};
    uint32_t m_destruct_tag;

    /**
     * key 为动态链接库实际绝对路径（不是软链接，也不是相对路径）
     * value为动态链接库加载地址
     */
    typedef std::map<std::string, addr_t> addr_map_t;
    addr_map_t m_addr_map;

    // 程序的绝对路径
    std::string m_exec_path;

    /**
     * 文件"/proc/xxx/maps"的路径，其中xxx为进程ID.
     * 该文件保存着共享库在进程空间的映射地址.
     */
    std::string m_maps_path;

public:
    AddrMapManager()
        : m_destruct_tag(CONSTRUCTED_TAG)
    {
        char file_path[PATH_MAX + 1] = {0};
        ::sprintf(file_path, "/proc/%d/maps", getpid());
        m_maps_path = file_path;

        char link_path[PATH_MAX + 1] = {0};
        char actual_path[PATH_MAX + 1] = {0};
        ::sprintf(link_path, "/proc/%d/exe",getpid());
        ::readlink(link_path, actual_path, PATH_MAX);
        m_exec_path = actual_path;

        load();

        // 程序本身
        m_addr_map.insert(std::pair<std::string, addr_t>(m_exec_path, 0));
    }

    ~AddrMapManager()
    {
        m_destruct_tag = DESTRUCTED_TAG;
    }

    /**
     * 检查结构体是否依然有效
     */
    bool is_valid() const
    {
        return NULL != this && m_destruct_tag == CONSTRUCTED_TAG;
    }

    /**
     * 获取应用程序绝对路径
     */
    const std::string& get_exec_path() const
    {
        return m_exec_path;
    }

	/**
     * 从文件 m_maps_path 中加载地址的映射，path为空表示加载所有。
     */
    void load(const std::string& path = std::string())
    {
        std::ifstream inf(m_maps_path.c_str());
        if (!inf)
            return;

        std::string str_line;
        while (getline(inf, str_line))
        {
            // Linux下的getline没法处理"\r\n"
            str_line = nut::trim(str_line, "\r\n");

            if (!path.empty())
            {
                if (parse_line(str_line, true, path))
                    break;
            }
            else // 只要是有效的，都装载
            {
                parse_line(str_line);
            }
        }
        inf.close();
    }

    /**
     * 找到某个共享库的起始地址
     */
    bool find(const std::string& path, addr_t *out_addr)
    {
        assert(NULL != out_addr);
        *out_addr = 0;

        addr_map_t::iterator iter = m_addr_map.find(path);
        if (iter == m_addr_map.end())
            return false;

        *out_addr = iter->second;
        return true;
    }

private:
    /**
     * 本函数用于获取从文件"/proc/进程ID/maps"中获取的一行的信息，形如：
     * 00577000-00578000 r-xp 00000000 08:01 160313     /usr/local/lib/libbtshare2.so
     * 它表示共享库在程序中的加载地址
     */
    bool parse_line(const std::string& str_line, bool fappoint_path = false, const std::string& path = std::string())
    {
        // parse share library file path
        std::string str_path;
        const std::string::size_type pos_begin = str_line.find('/');
        const std::string::size_type pos_end = str_line.rfind(".so"); // XXX 必须包含.so，不知存不存这样的共享库：文件名不包含".so"
        if (pos_begin != std::string::npos && pos_end != std::string::npos && pos_begin < pos_end)
        {
            str_path.assign(str_line.begin() + pos_begin, str_line.end());
            // 如果指定分析某一个共享库，则判断当前行是否为该库信息，如果不是则不处理
            if (fappoint_path && str_path.compare(path) != 0)
                return false;
        }

        // parse share library address
        std::string str_addr;
        std::string::size_type pos = str_line.find('-');
        if (pos != std::string::npos)
            str_addr.assign(str_line.begin(), str_line.begin() + pos);

        if (str_path.size() > 0 && str_addr.size() > 0)
        {
            char* pchEnd = NULL;
            addr_t addr = ::strtoul(str_addr.c_str(), &pchEnd, 16);
            addr_map_t::iterator iter = m_addr_map.find(str_path);
            // 据观察，第一个地址是最小的，所以之后的地址不需要了
            if (iter == m_addr_map.end())
                m_addr_map.insert(make_pair(str_path, addr));

            return true;
        }
        return false;
    }
};

}

#endif

#endif
