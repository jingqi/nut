/**
 * @file -
 * @author jingqi
 * @date 2013-10-03
 * @last-edit 2014-07-30 01:48:13 jingqi
 * @brief
 */

#ifndef ___HEADFILE_99EF5EA5_F01C_4802_A51B_9F51F3C43C84_
#define ___HEADFILE_99EF5EA5_F01C_4802_A51B_9F51F3C43C84_

#include <vector>
#include <string>
#include <map>
#include <fstream>

#include <nut/gc/gc.hpp>
#include <nut/platform/path.hpp>

#include "xmlelement.hpp"

namespace nut
{

class XmlDocument
{
    ref<XmlElement> m_root;

public:
    inline ref<XmlElement> getRoot() const
    {
        return m_root;
    }

    inline void setRoot(ref<XmlElement> root)
    {
        m_root = root;
    }

    void read(const std::string& filePath, bool ignore_text_blank = true)
    {
        if (!Path::exists(filePath))
            return;

        std::ifstream ifs(filePath.c_str());
        std::string line, all;
        bool first_line = true;
        while (getline(ifs, line))
        {
            // 兼容将windows下的换行拿到linux下使用导致的问题
            if (line.length() > 0 && '\r' == *line.rbegin())
                line.erase(line.length() - 1);

            if (!first_line)
                all.push_back('\n');
            all += line;
            first_line = false;
        }
        ifs.close();

        parse(all, ignore_text_blank);
    }

    /**
     * @param ignore_text_blank 忽略文本首尾的空白和换行
     */
    void parse(const std::string& s, bool ignore_text_blank = true)
    {
        m_root.clear();
        size_t i = s.find_first_of('>'); // skip header
        if (std::string::npos == i)
            return;
        i = s.find_first_of('<', i);
        if (std::string::npos == i)
            return;
        m_root = gc_new<XmlElement>();
        m_root->parse(s, i, ignore_text_blank);
    }

    inline void write(const std::string& filePath, bool format = true) const
    {
        std::ofstream ofs(filePath.c_str());
        std::string all;
        serielize(&all, format);
        ofs << all;
        ofs.flush();
        ofs.close();
    }

    /**
     * @param format 格式化输出，以便于阅读
     */
    void serielize(std::string *out, bool format = true) const
    {
        assert(NULL != out);
        const char *encoding = "UTF-8";
        *out += "<?xml version=\"1.0\" encoding=\"";
        *out += encoding;
        *out += "\"?>";
        if (m_root.isNull())
            return;
        if (format)
            out->push_back('\n');
        m_root->serielize(out, format ? 0 : -1);
    }
};

}

#endif
