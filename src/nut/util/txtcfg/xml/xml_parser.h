
#ifndef ___HEADFILE_303F68D8_9542_4BDD_A2C9_69764EE0AC70_
#define ___HEADFILE_303F68D8_9542_4BDD_A2C9_69764EE0AC70_

#include <assert.h>
#include <string>
#include <vector>

#include "element_handler.h"

namespace nut
{

class XmlParser
{
    struct ElementInfo
    {
        std::string name;
        XmlElementHandler *handler;

        ElementInfo(const std::string& n, XmlElementHandler *h)
            : name(n), handler(h)
        {}
    };
    std::vector<ElementInfo> m_elem_path;

    size_t m_line, m_column;

    // 状态机定义
    std::string m_tmp_name, m_tmp_value, m_tmp_encoded;
    int m_state;

private:
    bool input(char c);

    bool should_handle_child() const;
    bool should_handle_attribute() const;
    bool should_handle_text() const;
    bool should_handle_comment() const;

    void handle_child();
    void handle_attribute();
    void handle_text();
    void handle_comment();
    void handle_finish();
    bool check_finish();

    void force_finish();

public:
    /**
     * NODE: 根 handler 的删除操作需要外部自己管理
     */
    XmlParser(XmlElementHandler *root_handler);

    void reset(XmlElementHandler *root_handler);

    bool input(const char *s, int len = -1);

    bool finish();

    size_t line() const
    {
        return m_line;
    }

    size_t column() const
    {
        return m_column;
    }

    bool has_error() const;

    std::string error_message() const;
};

}

#endif
