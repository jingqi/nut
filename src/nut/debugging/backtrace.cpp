
#include <assert.h>
#include <stdlib.h> // for malloc(), free()
#include <iostream>

#include "../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#   include <process.h>
#   include <dbghelp.h>
#else
#   include <execinfo.h> // for backtrace() and backtrace_symbols()
#   include <limits.h> // for PATH_MAX
#endif

#include "../util/string/string_utils.h"
#include "../util/string/to_string.h"
#include "backtrace.h"
#include "proc_addr_maps.h"


// linux 下是否使用 ProcAddrMaps、addr2line 获取更详细的信息
#define USE_ADDR2LINE 1

// windows 下使用 dbghelp (需要额外连接 Dbghelp.lib) 解析符号
#define USE_DBGHELP 1

// 最大回溯栈层数
#define MAX_BACKTRACE 256

#if USE_DBGHELP && NUT_PLATFORM_CC_VC
#   pragma comment(lib, "Dbghelp.lib")
#endif

namespace nut
{

namespace
{

unsigned dec_width(int n)
{
    int count = 0;
    while (n > 0)
    {
        n /= 10;
        ++count;
    }
    return 0 == count ? 1 : count;
}

std::string fmt_seq(int v, unsigned width)
{
    const std::string num = llong_to_str(v);
    std::string prepand;
    while (prepand.length() + num.length() < width)
        prepand.push_back(' ');
    return prepand + num + " ";
}

}

#if NUT_PLATFORM_OS_WINDOWS

std::string Backtrace::backtrace(unsigned skip_top_frames)
{
    // Get calling stack
    void *trace[MAX_BACKTRACE];
    const USHORT trace_count = ::CaptureStackBackTrace(
        skip_top_frames, MAX_BACKTRACE, trace, nullptr);

    std::string ret;
#if USE_DBGHELP
    // Load symbols
    const HANDLE process = ::GetCurrentProcess();
    ::SymInitialize(process, nullptr, TRUE);
    ::SymSetOptions(SYMOPT_LOAD_LINES);

    const int MAX_FUNC_NAME_LENGTH = 1024;
    SYMBOL_INFO *symbol = (SYMBOL_INFO*) ::malloc(
        sizeof(SYMBOL_INFO) + sizeof(TCHAR) * (MAX_FUNC_NAME_LENGTH - 1));
    symbol->MaxNameLen = MAX_FUNC_NAME_LENGTH;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    IMAGEHLP_LINE64 line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    const unsigned width = dec_width(trace_count - 1);
    for (USHORT i = 0; i < trace_count; ++i)
    {
        if (!ret.empty())
            ret.push_back('\n');
        ret += fmt_seq(i, width);

        const DWORD64 address = (DWORD64) trace[i];
        if (!::SymFromAddr(process, address, nullptr, symbol))
        {
            ret += format("[0x%p]", trace[i]);
            continue;
        }

        DWORD displacement = 0;
        if (!::SymGetLineFromAddr64(process, address, &displacement, &line))
        {
            ret += format("[0x%p] %s", trace[i], symbol->Name);
            continue;
        }

        ret += " file \"";
        ret += line.FileName;
        ret += "\", line ";
        ret += llong_to_str(line.LineNumber);
        ret += ", in ";
        ret += symbol->Name;
    }

    ::free(symbol);
    ::SymCleanup(process);
#else
    const unsigned width = dec_width(trace_count - 1);
    for (USHORT i = 0; i < trace_count; ++i)
    {
        if (!ret.empty())
            ret.push_back('\n');
        ret += fmt_seq(i, width);
        ret += format("[0x%p]", trace[i]);
    }
#endif

    return ret;
}

#elif NUT_PLATFORM_OS_LINUX && USE_ADDR2LINE

namespace
{

/*
 * 构造用于调用addr2line的命令。
 */
std::string make_addr2line_cmd(ProcAddrMaps::addr_type addr, const std::string& module_path)
{
    return format("addr2line 0x%X -C -f -e %s", addr, module_path.c_str());
}

/*
 * 调用 addr2line，查询地址，获得源文件、行号和函数名
 */
std::string call_addr2line(const std::string& cmd)
{
    std::string ret;
    FILE* fp = ::popen(cmd.c_str(), "r");
    if (nullptr == fp)
        return ret;

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

    if ((func.empty() || func == "??") && (file.empty() || file == "??") &&
        (line.empty() || line == "0"))
        return ret;

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

    const unsigned width = dec_width(trace_count - skip_top_frames - 1);
    for (int i = skip_top_frames; i < trace_count; i++)
    {
        if (!ret.empty())
            ret.push_back('\n');
        ret += fmt_seq(i - skip_top_frames, width);

        if (nullptr == trace_strs[i])
            continue;

        std::string module_path;
        ProcAddrMaps::addr_type func_addr;
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
        const std::string line = call_addr2line(cmd);
        if (line.empty())
            ret += trace_strs[i];
        else
            ret += line;
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

    const unsigned width = dec_width(trace_count - skip_top_frames - 1);
    for (int i = skip_top_frames; i < trace_count; ++i)
    {
        if (!ret.empty())
            ret.push_back('\n');
        ret += fmt_seq(i - skip_top_frames, width);

        const char *tr = trace_strs[i];
        if (nullptr == tr)
            continue;

#if NUT_PLATFORM_OS_MACOS
        // NOTE macOS 系统上会自己在前面加上栈层号，需要换成我们自己的
        size_t pos = 0;
        while (' ' == tr[pos] || ('0' <= tr[pos] && tr[pos] <= '9'))
            ++pos;
        ret += tr + pos;
#else
        ret += tr;
#endif
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
