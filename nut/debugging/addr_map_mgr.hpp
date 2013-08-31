/**
 * @file -
 * @author jingqi
 * @date 2013-08-30
 * @last-edit 2013-08-30 22:32:57 jingqi
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

#include <nut/util/string/stringutil.hpp>

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
        char filePath[PATH_MAX + 1] = {0};
        ::sprintf(filePath, "/proc/%d/maps", getpid());
        m_maps_path = filePath;

        char linkPath[PATH_MAX + 1] = {0};
        char actualPath[PATH_MAX + 1] = {0};
        ::sprintf(linkPath, "/proc/%d/exe",getpid());
        ::readlink(linkPath, actualPath, PATH_MAX);
        m_exec_path = actualPath;

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
    inline bool isValid() const
    {
        return NULL != this && m_destruct_tag == CONSTRUCTED_TAG;
    }

    /**
     * 获取应用程序绝对路径
     */
    inline const std::string& getExecPath() const
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

        std::string strLine;
        while (getline(inf, strLine))
        {
            // Linux下的getline没法处理"\r\n"
            strLine = nut::trim(strLine, "\r\n");

            if (!path.empty())
            {
                if (parseLine(strLine, true, path))
                    break;
            }
            else // 只要是有效的，都装载
            {
                parseLine(strLine);
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
    bool parseLine(const std::string& strLine, bool fappointPath = false, const std::string& path = std::string())
    {
        // parse share library file path
        std::string strPath;
        const std::string::size_type posBegin = strLine.find('/');
        const std::string::size_type posEnd = strLine.rfind(".so"); // XXX 必须包含.so，不知存不存这样的共享库：文件名不包含".so"
        if (posBegin != std::string::npos && posEnd != std::string::npos && posBegin < posEnd)
        {
            strPath.assign(strLine.begin() + posBegin, strLine.end());
            // 如果指定分析某一个共享库，则判断当前行是否为该库信息，如果不是则不处理
            if (fappointPath && strPath.compare(path) != 0)
                return false;
        }

        // parse share library address
        std::string strAddr;
        std::string::size_type pos = strLine.find('-');
        if (pos != std::string::npos)
            strAddr.assign(strLine.begin(), strLine.begin() + pos);

        if (strPath.size() > 0 && strAddr.size() > 0)
        {
            char* pchEnd = NULL;
            addr_t addr = ::strtoul(strAddr.c_str(), &pchEnd, 16);
            addr_map_t::iterator iter = m_addr_map.find(strPath);
            // 据观察，第一个地址是最小的，所以之后的地址不需要了
            if (iter == m_addr_map.end())
                m_addr_map.insert(make_pair(strPath, addr));

            return true;
        }
        return false;
    }
};

}

#endif

#endif
