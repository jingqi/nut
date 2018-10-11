
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
private:
    class ElementInfo
    {
    public:
        ElementInfo(const std::string& n, XmlElementHandler *h)
            : name(n), handler(h)
        {}

    public:
        std::string name;
        XmlElementHandler *handler = nullptr;
    };

public:
    /**
     * NODE: 根 handler 的删除操作需要外部自己管理
     */
    explicit XmlParser(XmlElementHandler *root_handler);

    void reset(XmlElementHandler *root_handler);

    /**
     * @return false if error
     */
    bool input(const char *s, int len = -1);

    /**
     * @return false if error
     */
    bool finish();

    size_t line() const;
    size_t column() const;

    bool has_error() const;

    std::string error_message() const;

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

private:
    std::vector<ElementInfo> _elem_path;

    size_t _line = 1, _column = 1;

    // 解析状态机定义
    enum class State
    {
        InText, // 在 text 中
        InTextEncode, // 在 text 的转义符中
        JustAfterLessSign, // 紧接着 '<'
        ExpectElemNameFirstChar, // 预期 element 名称的第一个字符
        InElemName, // 在 element 名称中
        ExpectSpaceBeforeAttrNameFirstChar, // 在 attribute 名字的第一个字符前，至少匹配一个空格
        ExpectAttrNameFirstChar, // 预期 attribute 名称的第一个字符
        InAttrName, // 在 attribute 名称中
        ExpectEqual, // 预期 '＝' 字符
        ExpectFirstQuot, // 预期第一个双引号
        InAttrValue, // 在 attribute 的值中
        InAttrValueEncode, // 在 attribute 的值中的转义符中
        ExpectImmediatGreaterSignAndFinishElem, // 预期立即的 '>' 字符，然后结束 element
        ExpectFirstBar, // 预期注释中的第一个 '-' 符号
        ExpectSecondBar, // 预期注释中的第二个 '-' 符号
        InComment, // 在注释中
        ExpectLastBar, // 预期注释中的最后一个 '-'
        ExpectGreaterSignAndFinishComment, // 预期 '>' 字符，然后结束注释
        ExpectElemNameFirstCharAndFinishElem, // 预期第一个字符，然后结束 element
        InElemNameAndFinishElem, // 在 element 名称中，然后结束 element
        ExpectGreaterSignAndFinishElem, // 预期 '>' 然后 element 结束
        InError // 出错
    } _state;
    std::string _tmp_name, _tmp_value, _tmp_encoded;
};

}

#endif
