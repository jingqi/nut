﻿
#include <nut/platform/platform.h>

#include "addr_map_mgr.h"

#if NUT_PLATFORM_OS_LINUX

// 构造后的 tag
#define CONSTRUCTED_TAG 0xA5A55A5A

// 析构后的 tag
#define DESTRUCTED_TAG 0xFEDAADEF

namespace nut
{

AddrMapManager::AddrMapManager()
    : _destruct_tag(CONSTRUCTED_TAG)
{
    char file_path[PATH_MAX];
    safe_snprintf(file_path, PATH_MAX, "/proc/%d/maps", getpid());
    _maps_path = file_path;

    char link_path[PATH_MAX];
    char actual_path[PATH_MAX] = {0};
    safe_snprintf(link_path, PATH_MAX, "/proc/%d/exe",getpid());
    ::readlink(link_path, actual_path, PATH_MAX);
    _exec_path = actual_path;

    load();

    // 程序本身
    _addr_map.insert(std::pair<std::string, addr_type>(_exec_path, 0));
}

AddrMapManager::~AddrMapManager()
{
    _destruct_tag = DESTRUCTED_TAG;
}

bool AddrMapManager::is_valid() const
{
    return nullptr != this && _destruct_tag == CONSTRUCTED_TAG;
}

const std::string& AddrMapManager::get_exec_path() const
{
    return _exec_path;
}

void AddrMapManager::load(const std::string& path)
{
    std::ifstream inf(_maps_path.c_str());
    if (!inf)
        return;

    std::string str_line;
    while (getline(inf, str_line))
    {
        // Linux下的getline没法处理"\r\n"
        const std::string s = trim(str_line, "\r\n");

        if (!path.empty())
        {
            if (parse_line(s, true, path))
                break;
        }
        else // 只要是有效的，都装载
        {
            parse_line(s);
        }
    }
    inf.close();
}

bool AddrMapManager::find(const std::string& path, addr_type *out_addr)
{
    assert(nullptr != out_addr);
    *out_addr = 0;

    addr_map_type::iterator iter = _addr_map.find(path);
    if (iter == _addr_map.end())
        return false;

    *out_addr = iter->second;
    return true;
}

bool AddrMapManager::parse_line(const std::string& str_line, bool fappoint_path, const std::string& path)
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
        char* pch_end = nullptr;
        addr_type addr = ::strtoul(str_addr.c_str(), &pch_end, 16);
        addr_map_type::iterator iter = _addr_map.find(str_path);
        // 据观察，第一个地址是最小的，所以之后的地址不需要了
        if (iter == _addr_map.end())
            _addr_map.insert(make_pair(str_path, addr));

        return true;
    }
    return false;
}

}

#endif
