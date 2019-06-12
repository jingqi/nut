
#ifndef ___HEADFILE_303F68D8_9542_4BDD_A2C9_69764EE0AC70_
#define ___HEADFILE_303F68D8_9542_4BDD_A2C9_69764EE0AC70_

#include <assert.h>
#include <stdint.h>
#include <string>
#include <vector>

#include "../../../nut_config.h"
#include "element_handler.h"


namespace nut
{

class NUT_API XmlParser
{
private:
    class ElementInfo
    {
    public:
        ElementInfo(const std::string& n, XmlElementHandler *h) noexcept
            : name(n), handler(h)
        {}

    public:
        std::string name;
        XmlElementHandler *handler = nullptr;
    };

public:
    /**
     * NODE 根 handler 的删除操作需要外部自己管理
     */
    explicit XmlParser(XmlElementHandler *root_handler) noexcept;

    void reset(XmlElementHandler *root_handler) noexcept;

    /**
     * @return false if error
     */
    bool input(const char *s, int len = -1) noexcept;

    /**
     * @return false if error
     */
    bool finish() noexcept;

    size_t line() const noexcept;
    size_t column() const noexcept;

    bool has_error() const noexcept;

    std::string error_message() const noexcept;

private:
    bool input(char c) noexcept;

    bool should_handle_child() const noexcept;
    bool should_handle_attribute() const noexcept;
    bool should_handle_text() const noexcept;
    bool should_handle_comment() const noexcept;

    void handle_child() noexcept;
    void handle_attribute() noexcept;
    void handle_text() noexcept;
    void handle_comment() noexcept;
    void handle_finish() noexcept;
    bool check_finish() noexcept;

    void force_finish() noexcept;

private:
    std::vector<ElementInfo> _elem_path;

    size_t _line = 1, _column = 1;

    // 解析状态机
    enum class State : uint8_t;

    State _state;
    std::string _tmp_name, _tmp_value, _tmp_encoded;
};

}

#endif
