
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

    // 解析状态机定义
    enum class State
    {
        IN_TEXT, // 在 text 中
        IN_TEXT_ENCODE, // 在 text 的转义符中
        JUST_AFTER_LESS_SIGN, // 紧接着 '<'
        EXPECT_ELEM_NAME_FIRST_CHAR, // 预期 element 名称的第一个字符
        IN_ELEM_NAME, // 在 element 名称中
        EXPECT_SPACE_BEFORE_ATTR_NAME_FIRST_CHAR, // 在 attribute 名字的第一个字符前，至少匹配一个空格
        EXPECT_ATTR_NAME_FIRST_CHAR, // 预期 attribute 名称的第一个字符
        IN_ATTR_NAME, // 在 attribute 名称中
        EXPECT_EQUAL, // 预期 '＝' 字符
        EXPECT_FIRST_QUOT, // 预期第一个双引号
        IN_ATTR_VALUE, // 在 attribute 的值中
        IN_ATTR_VALUE_ENCODE, // 在 attribute 的值中的转义符中
        EXPECT_IMMEDIAT_GREATER_SIGN_AND_FINISH_ELEM, // 预期立即的 '>' 字符，然后结束 element
        EXPECT_FIRST_BAR, // 预期注释中的第一个 '-' 符号
        EXPECT_SECOND_BAR, // 预期注释中的第二个 '-' 符号
        IN_COMMENT, // 在注释中
        EXPECT_LAST_BAR, // 预期注释中的最后一个 '-'
        EXPECT_GREATER_SIGN_AND_FINISH_COMMENT, // 预期 '>' 字符，然后结束注释
        EXPECT_ELEM_NAME_FIRST_CHAR_AND_FINISH_ELEM, // 预期第一个字符，然后结束 element
        IN_ELEM_NAME_AND_FINISH_ELEM, // 在 element 名称中，然后结束 element
        EXPECT_GREATER_SIGN_AND_FINISH_ELEM, // 预期 '>' 然后 element 结束
        IN_ERROR // 出错
    } _state;
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
