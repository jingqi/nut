/**
 * @file -
 * @author jingqi
 * @date 2013-08-30
 * @last-edit 2013-08-30 22:29:53 jingqi
 * @brief
 */

#ifndef ___HEADFILE_B3E81AC9_35E7_4ADF_A586_39A6E77844E7_
#define ___HEADFILE_B3E81AC9_35E7_4ADF_A586_39A6E77844E7_

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_LINUX)

#include <assert.h>
#include <execinfo.h>
#include <stdlib.h>
#include <string>

// 是否使用 AddrMapManager 获取更详细的信息
#define USE_ADDR_MAP 1

#if USE_ADDR_MAP
#   include "addr_map_mgr.hpp"
#endif

namespace nut
{

/**
 * 打印调用栈
 *
 * NOTE:
 *    使用 backtrace_symbols() 函数要求编译时给 g++ 一个 -rdynamic 编译参数
 */
class Backtrace
{
    // 最大回溯层数
    enum { MAX_BACKTRACE = 100 };

#if USE_ADDR_MAP
private:
    typedef AddrMapManager::addr_t addr_t;

    static AddrMapManager& getAddrMapMgr()
    {
        static AddrMapManager ret;
        return ret;
    }

public:
    /**
     * @return >=0， 调用栈层数
     *         <0， 出错
     */
    static int backtrace(std::string *out)
    {
        assert(NULL != out);
        out->clear();

        // XXX 有可能该方法在静态域中被调用，而此时 AddrMapManager 已经被析构
        AddrMapManager& addrMap = getAddrMapMgr();
        if (!addrMap.isValid())
            return -1;

        void *trace[MAX_BACKTRACE + 1];
        const size_t trace_size = ::backtrace(trace, MAX_BACKTRACE + 1);
        char **messages = ::backtrace_symbols(trace, trace_size);
        if (NULL == messages)
            return -1;

        // 从1开始，不返回本函数的信息
        for (size_t i = 1; i < trace_size; i++)
        {
            std::string func_module_path;
            addr_t func_addr;
            parseBacktraceSymbol(messages[i], &func_module_path, &func_addr);

            std::string cmd;
            addr_t addr = 0;
            bool bExists = addrMap.find(func_module_path, &addr);
            if (!bExists)
            {
                // 处理软链接和相对路径的情况，因为在文件/proc/XXX/maps中存的是实际文件地址。
                getRealPath(&func_module_path);

                // 指定装载
                addrMap.load(func_module_path);
                // 再次查询
                bExists = addrMap.find(func_module_path, &addr);
                // 没办法了，只能当做当前程序来处理，不管它是不是共享库了
                if (!bExists)
                {
                    cmd = makeAddr2LineCmd(func_addr, addrMap.getExecPath());
                }
                else // 装载成功
                {
                    cmd = makeAddr2LineCmd(func_addr - addr, func_module_path);
                }
            }
            else // iter != gAddrMap.end()
            {
                cmd = makeAddr2LineCmd(func_addr - addr, func_module_path);
            }

            // 调用addr2line
            callAddr2Line(cmd, out);

        }

        ::free(messages); // 必须调用

        return trace_size - 1; // 不包括本函数
    }

private:
    /*
     * 构造用于调用addr2line的命令。
     */
    static std::string makeAddr2LineCmd(addr_t addr, const std::string& strPath)
    {
        char buf[50] = {0};
        sprintf(buf, "0x%X", addr);
        std::string cmd = "addr2line ";
        cmd += buf;
        cmd += " -C -f -e ";
        cmd += strPath;
        return cmd;
    }

    /*
     * 调用addr2line，查询地址，获得源文件、行号和函数名
     */
    static void callAddr2Line(const std::string& cmd, std::string *out_result)
    {
        FILE* fp = popen(cmd.c_str(), "r");
        if (!fp)
        {
            perror("popen");
        }
        else
        {
            std::string funName;
            std::string sourceLine;
            char buf;
            char lastBuf = '\0';
            fseek(fp, 0, 0);
            while(fread(&buf, 1, 1, fp))
            {
                lastBuf = buf;
                if (buf == '\r' || buf == '\n')
                    break;
                funName.push_back(buf);
            }
            while(fread(&buf, 1, 1, fp))
            {
                lastBuf = buf;
                if (buf == '\r')
                    break;
                if (buf == '\n' && lastBuf != '\r')
                    break;
                sourceLine.push_back(buf);
            }
            pclose(fp);
            
            if (out_result->empty())
                *out_result += sourceLine + std::string(":  ") + funName;
            else
                *out_result += std::string("\n") + sourceLine + std::string(":  ") + funName;
        }
    }
    
    /*
     * 将路径strPath转化成绝对路径，这会处理软链接和相对路径
     */
    static void getRealPath(std::string *in_out_strPath)
    {
        assert(NULL != in_out_strPath);
        char resolved_path[PATH_MAX + 1] = {0};
        if (realpath(in_out_strPath->c_str(), resolved_path))
            *in_out_strPath = resolved_path;
    }
    
    /**
     * 本函数用于分析从backtrace_symbols获取到的信息, 形如: 
     * /usr/local/lib/libbtshare.so(_Z16LogBacktraceInfov+0x38) [0x5fa269]
     */
    static void parseBacktraceSymbol(const char* backtraceSymbol, std::string *out_module_path, addr_t *out_addr)
    {
        assert(NULL != backtraceSymbol && NULL != out_module_path && NULL != out_addr);
        out_module_path->clear();
        *out_addr = 0;

        std::string str(backtraceSymbol);

        // parse path of shared library
        const size_t pos = str.find('(');
        if (pos != std::string::npos)
            out_module_path->assign(str.begin(), str.begin() + pos);

        // parse address
        const size_t posBegin = str.find('['), posEnd = str.find(']');
        if (posBegin != std::string::npos && posBegin < posEnd)
        {
            std::string strAddr;
            strAddr.assign(str.begin() + posBegin + 1, str.begin() + posEnd);
            char* pchEnd = NULL;
            *out_addr = ::strtoul(strAddr.c_str(), &pchEnd, 16);
        }
    }
#else
public:
    /**
     * @return >=0， 调用栈层数
     *         <0， 出错
     */
    static int backtrace(std::string *out)
    {
        assert(NULL != out);
        out->clear();

        void *buffer[MAX_BACKTRACE + 1];
        const int nptrs = ::backtrace(buffer, MAX_BACKTRACE + 1);
        char **strs = ::backtrace_symbols(buffer, nptrs);
        if (NULL == strs) // error
            return -1;
        for (int i = 1; i < nptrs; ++i) // 不包含本函数的调用地址
        {
            *out += strs[i];
            *out += "\n";
        }
        ::free(strs);
        return nptrs - 1;
    }
#endif
};

}

#undef USE_ADDR_MAP

#endif

#endif
