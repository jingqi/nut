
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_LINUX

#include <assert.h>
#include <limits.h> // for PATH_MAX
#include <unistd.h> // for getpid()
#include <sys/types.h> // for pid_t
#include <fstream>

#include <nut/util/string/string_utils.h>
#include <nut/platform/os.h>

#include "proc_addr_maps.h"


// 构造后的 tag
#define CONSTRUCTED_TAG 0xa5a55a5a

// 析构后的 tag
#define DESTRUCTED_TAG 0xdeadbeef

namespace nut
{

ProcAddrMaps& ProcAddrMaps::instance()
{
    static ProcAddrMaps ins;
    return ins;
}

ProcAddrMaps::ProcAddrMaps()
    : _destruct_tag(CONSTRUCTED_TAG)
{
    const pid_t pid = ::getpid();
    _exec_path = OS::read_link(format("/proc/%d/exe", pid));
    _maps_path = format("/proc/%d/maps", pid);

    load();

    // 程序本身默认从 0 开始, linux 可以 ALSR (地址空间随机化)，会导致起始地址不是 0
    _addr_map.emplace(_exec_path, 0);
}

ProcAddrMaps::~ProcAddrMaps()
{
    _destruct_tag = DESTRUCTED_TAG;
}

bool ProcAddrMaps::is_valid() const
{
    return nullptr != this && _destruct_tag == CONSTRUCTED_TAG;
}

const std::string& ProcAddrMaps::get_exec_path() const
{
    return _exec_path;
}

void ProcAddrMaps::load(const std::string& module_path)
{
    std::ifstream input(_maps_path.c_str());
    if (!input)
        return;

    std::string line;
    while (getline(input, line))
    {
        // Linux下的 getline() 没法处理"\r\n"
        line = trim(line, "\r\n");

        if (!module_path.empty())
        {
            // 找到指定 module 则终止
            if (parse_line(line, module_path))
                break;
        }
        else
        {
            parse_line(line);
        }
    }
    input.close();
}

bool ProcAddrMaps::parse_line(const std::string& line, const std::string& module_path)
{
    // Parse module file path
    std::string::size_type pos = line.find('/');
    if (std::string::npos == pos)
        return false;
    std::string path(line.begin() + pos, line.end());
    if (ends_with(path, " (deleted)"))
        path.resize(path.length() - 10);

    // 如果指定分析某一个共享库，则判断当前行是否为该库信息，如果不是则不处理
    if (!module_path.empty() && path != module_path)
        return false;

    // 据观察，第一个地址是最小的，所以之后的地址不需要了
    if (_addr_map.find(path) != _addr_map.end())
        return true;

    // Parse module virtual memory starting address
    pos = line.find('-');
    if (std::string::npos == pos)
        return false;
    std::string vm_start(line.begin(), line.begin() + pos);

    char* pch_end = nullptr;
    const addr_type vm_start_addr = ::strtoul(vm_start.c_str(), &pch_end, 16);
    _addr_map.emplace(path, vm_start_addr);

    return true;
}

bool ProcAddrMaps::find(const std::string& module_path, addr_type *out_addr) const
{
    assert(nullptr != out_addr);
    *out_addr = 0;

    addr_map_type::const_iterator iter = _addr_map.find(module_path);
    if (iter == _addr_map.end())
        return false;

    *out_addr = iter->second;
    return true;
}

}

#endif
