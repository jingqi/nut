
#ifndef ___HEADFILE_303F68D8_9542_4BDD_A2C9_69764EE0AC70_
#define ___HEADFILE_303F68D8_9542_4BDD_A2C9_69764EE0AC70_

#include <assert.h>
#include <string>
#include <vector>

#include "../../../nut_config.h"
#include "element_handler.h"


namespace nut
{

class NUT_API XmlParser
{
    struct ElementInfo
    {
        std::string name;
        XmlElementHandler *handler = nullptr;

        ElementInfo(const std::string& n, XmlElementHandler *h)
            : name(n), handler(h)
        {}
    };
    std::vector<ElementInfo> _elem_path;

    size_t _line = 1, _column = 1;

    // 状态机定义
    enum class State;
    State _state;
    std::string _tmp_name, _tmp_value, _tmp_encoded;

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
    explicit XmlParser(XmlElementHandler *root_handler);

    void reset(XmlElementHandler *root_handler);

    bool input(const char *s, int len = -1);

    bool finish();

    size_t line() const
    {
        return _line;
    }

    size_t column() const
    {
        return _column;
    }

    bool has_error() const;

    std::string error_message() const;
};

}

#endif
