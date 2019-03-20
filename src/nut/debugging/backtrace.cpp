
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_MAC || NUT_PLATFORM_OS_LINUX

#include <assert.h>
#include <execinfo.h> // for backtrace() and backtrace_symbols()
#include <stdlib.h> // for free()
#include <limits.h> // for PATH_MAX
#include <iostream>

#include <nut/util/string/string_utils.h>

#include "backtrace.h"
#include "proc_addr_maps.h"


// 是否使用 AddrMapManager 获取更详细的信息
#define NUT_BACKTRACE_USE_ADDR_MAP (NUT_PLATFORM_OS_LINUX && 1)

// 最大回溯栈层数
#define MAX_BACKTRACE 256

namespace nut
{

#if NUT_BACKTRACE_USE_ADDR_MAP

namespace
{

/*
 * 构造用于调用addr2line的命令。
 */
std::string make_addr2line_cmd(ProcAddrMaps::addr_type addr, const std::string& module_path)
{
    char buf[50] = {0};
    safe_snprintf(buf, 50, "0x%X", addr);
    std::string cmd = "addr2line ";
    cmd += buf;
    cmd += " -C -f -e ";
    cmd += module_path;
    return cmd;
}

/*
 * 调用 addr2line，查询地址，获得源文件、行号和函数名
 */
std::string call_addr2line(const std::string& cmd)
{
    std::string ret;
    FILE* fp = ::popen(cmd.c_str(), "r");
    if (!fp)
    {
        ::perror("popen");
        return ret;
    }

    /*
     * 正常 addr2line 输出格式:
     * func()
     * file/path.c:218
     *
     * 失败输出：
     * ??
     * ??:0
     */
    std::string func;
    char c;
    ::fseek(fp, 0, 0);
    while (::fread(&c, 1, 1, fp) > 0)
    {
        if ('\r' == c || '\n' == c)
            break;
        func.push_back(c);
    }

    std::string file;
    while (true)
    {
        const char lastc = c;
        if (::fread(&c, 1, 1, fp) == 0)
            break;

        if (':' == c)
            break;
        if ('\r' == c || '\n' == c)
        {
            if ('\r' == lastc && '\n' == lastc)
                continue;
            break;
        }
        file.push_back(c);
    }

    std::string line;
    while (true)
    {
        const char lastc = c;
        if (::fread(&c, 1, 1, fp) == 0)
            break;

        if ('\r' == c || '\n' == c)
        {
            if ('\r' == lastc && '\n' == lastc)
                continue;
            break;
        }
        line.push_back(c);
    }

    ::pclose(fp);

    ret += "file \"";
    ret += file;
    ret += "\", line ";
    ret += line;
    ret += ", in ";
    ret += func;
    return ret;
}

/*
 * 将路径 str_path 转化成绝对路径，这会处理软链接和相对路径
 */
std::string get_real_path(const std::string& path)
{
    char resolved_path[PATH_MAX + 1] = {0};
    if (::realpath(path.c_str(), resolved_path))
        return resolved_path;
    return path;
}

/**
 * 本函数用于分析从backtrace_symbols获取到的信息, 形如:
 * /usr/local/lib/libbtshare.so(_Z16LogBacktraceInfov+0x38) [0x5fa269]
 *
 * @param backtrace_symbols [in]
 * @param module_path [out] e.g. '/usr/local/lib/libbtshare.so'
 * @param addr [out] e.g. 0x5fa269
 */
void parse_backtrace_symbol(const std::string& backtrace_symbol,
                            std::string *module_path, ProcAddrMaps::addr_type *addr)
{
    assert(nullptr != module_path && nullptr != addr);
    *addr = 0;

    std::string str(backtrace_symbol);

    // parse path of shared library
    const size_t pos = backtrace_symbol.find('(');
    if (std::string::npos != pos)
        *module_path = std::string(backtrace_symbol.begin(), backtrace_symbol.begin() + pos);

    // parse address
    const size_t begin_pos = backtrace_symbol.find('['),
        end_pos = backtrace_symbol.find(']');
    if (std::string::npos != begin_pos && begin_pos < end_pos)
    {
        std::string str_addr(backtrace_symbol.begin() + begin_pos + 1,
                             backtrace_symbol.begin() + end_pos);
        char* pch_end = nullptr;
        *addr = ::strtoul(str_addr.c_str(), &pch_end, 16);
    }
}

}

std::string Backtrace::backtrace(unsigned skip_top_frames)
{
    std::string ret;

    // XXX 有可能该方法在静态域中被调用，而此时 ProcAddrMaps 已经被析构
    ProcAddrMaps& addr_map = ProcAddrMaps::instance();
    if (!addr_map.is_valid())
        return ret;

    void *trace[MAX_BACKTRACE];
    const int trace_count = ::backtrace(trace, MAX_BACKTRACE);
    char **trace_strs = ::backtrace_symbols(trace, trace_count);
    if (nullptr == trace_strs)
        return ret;

    // 从1开始，不返回本函数的信息
    for (int i = skip_top_frames; i < trace_count; i++)
    {
        std::string module_path;
        ProcAddrMaps::addr_type func_addr;
        if (nullptr == trace_strs[i])
            continue;
        parse_backtrace_symbol(trace_strs[i], &module_path, &func_addr);

        std::string cmd;
        ProcAddrMaps::addr_type start_addr = 0;
        bool found = addr_map.find(module_path, &start_addr);
        if (found)
        {
            cmd = make_addr2line_cmd(func_addr - start_addr, module_path);
        }
        else
        {
            // 处理软链接和相对路径的情况，因为在文件/proc/xxx/maps中存的是实际
            // 文件地址。
            module_path = get_real_path(module_path);

            // 模块可能是后来动态加载的，需要再次装载、查询 /proc/xxx/maps
            addr_map.load(module_path);
            found = addr_map.find(module_path, &start_addr);

            if (found)
            {
                cmd = make_addr2line_cmd(func_addr - start_addr, module_path);
            }
            else
            {
                // 没办法了，只能当做当前程序来处理，不管它是不是共享库了
                cmd = make_addr2line_cmd(func_addr, addr_map.get_exec_path());
            }
        }

        // 调用 addr2line
        if (!ret.empty())
            ret.push_back('\n');
        const std::string rs = call_addr2line(cmd);
        if (rs == "file \"??\", line 0, in ??")
            ret += trace_strs[i];
        else
            ret += call_addr2line(cmd);
    }

    ::free(trace_strs); // 必须调用

    return ret;
}

#else

std::string Backtrace::backtrace(unsigned skip_top_frames)
{
    std::string ret;

    void *trace[MAX_BACKTRACE];
    const int trace_count = ::backtrace(trace, MAX_BACKTRACE);
    char **trace_strs = ::backtrace_symbols(trace, trace_count);
    if (nullptr == trace_strs) // error
        return ret;
    for (int i = skip_top_frames; i < trace_count; ++i)
    {
        ret += trace_strs[i];
        ret.push_back('\n');
    }
    ::free(trace_strs);
    return ret;
}

#endif

void Backtrace::print_stack()
{
    std::cerr << std::endl << backtrace(1) << std::endl;
}

}

#endif
