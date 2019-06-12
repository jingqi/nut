﻿
#include <assert.h>

#include "xml_parser.h"


// 转义字符最大长度
#define MAX_ENCODED_LEN 6

namespace nut
{

static const char* decode(const std::string& e) noexcept
{
    if (e == "&amp;")
        return "&";
    else if (e == "&quot;")
        return "\"";
    else if (e == "&lt;")
        return "<";
    else if (e == "&gt;")
        return ">";
    else
        return e.c_str(); // decode failed
}

static constexpr bool is_space(char c) noexcept
{
    return ' ' == c || '\t' == c || '\r' == c || '\n' == c;
}

static bool is_name_char(char c) noexcept
{
    if ('a' <= c && c <= 'z')
        return true;
    if ('A' <= c && c <= 'Z')
        return true;
    if ('0' <= c && c <= '9')
        return true;
    if ('_' == c || '.' == c)
        return true;
    return false;
}

enum class XmlParser::State : uint8_t
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
};

bool XmlParser::input(char c) noexcept
{
    switch (_state)
    {
    case State::InText:
        switch (c)
        {
        case '<':
            handle_text();
            _state = State::JustAfterLessSign;
            break;

        case '&':
            _tmp_encoded.push_back('&');
            _state = State::InTextEncode;
            break;

        default:
            if (should_handle_text())
                _tmp_value.push_back(c);
            break;
        }
        break;

    case State::InTextEncode:
        switch (c)
        {
        case '<':
            handle_text();
            _state = State::JustAfterLessSign;
            break;

        case '&':
            if (should_handle_text())
                _tmp_value += _tmp_encoded;
            _tmp_encoded.clear();
            _tmp_encoded.push_back('&');
            break;

        case ';':
            if (should_handle_text())
            {
                _tmp_encoded.push_back(';');
                _tmp_value += decode(_tmp_encoded);
            }
            _tmp_encoded.clear();
            _state = State::InText;
            break;

        default:
            _tmp_encoded.push_back(c);
            if (_tmp_encoded.length() >= MAX_ENCODED_LEN)
            {
                if (should_handle_text())
                    _tmp_value += _tmp_encoded;
                _tmp_encoded.clear();
                _state = State::InText;
            }
            break;
        }
        break;

    case State::JustAfterLessSign:
        switch (c)
        {
        case '/':
            _state = State::ExpectElemNameFirstCharAndFinishElem;
            break;

        case '!':
            _state = State::ExpectFirstBar;
            break;

        default:
            if (is_space(c))
            {
                _state = State::ExpectElemNameFirstChar;
                break;
            }
            if (is_name_char(c))
            {
                if (should_handle_child())
                    _tmp_name.push_back(c);
                _state = State::InElemName;
                break;
            }
            _state = State::InError;
            _tmp_value = "invalid charactor \'";
            _tmp_value.push_back(c);
            _tmp_value += "\' for start of element name";
            return false; // invalid state
        }
        break;

    case State::ExpectElemNameFirstChar:
        if (is_space(c))
            break;
        if (is_name_char(c))
        {
            if (should_handle_child())
                _tmp_name.push_back(c);
            _state = State::InElemName;
            break;
        }
        _state = State::InError;
        _tmp_value = "invalid charactor \'";
        _tmp_value.push_back(c);
        _tmp_value += "\' for start of element name";
        return false; // invalid state

    case State::InElemName:
        switch (c)
        {
        case '/':
            handle_child();
            _state = State::ExpectImmediatGreaterSignAndFinishElem;
            break;

        case '>':
            handle_child();
            _state = State::InText;
            break;

        default:
            if (is_space(c))
            {
                handle_child();
                _state = State::ExpectAttrNameFirstChar;
                break;
            }
            if (is_name_char(c))
            {
                if (should_handle_child())
                    _tmp_name.push_back(c);
                break;
            }
            _state = State::InError;
            _tmp_value = "invalid charactor \'";
            _tmp_value.push_back(c);
            _tmp_value += "\' in element name";
            return false; // invalid state
        }
        break;

    case State::ExpectSpaceBeforeAttrNameFirstChar:
        switch (c)
        {
        case '/':
            _state = State::ExpectImmediatGreaterSignAndFinishElem;
            break;

        case '>':
            _state = State::InText;
            break;

        default:
            if (is_space(c))
            {
                _state = State::ExpectAttrNameFirstChar;
                break;
            }
            _state = State::InError;
            _tmp_value = "expect SPACE but got a \'";
            _tmp_value.push_back(c);
            _tmp_value += "\'";
            return false; // invalid state
        }
        break;

    case State::ExpectAttrNameFirstChar:
        switch (c)
        {
        case '/':
            _state = State::ExpectImmediatGreaterSignAndFinishElem;
            break;

        case '>':
            _state = State::InText;
            break;

        default:
            if (is_space(c))
                break;
            if (is_name_char(c))
            {
                if (should_handle_attribute())
                    _tmp_name.push_back(c);
                _state = State::InAttrName;
                break;
            }
            _state = State::InError;
            _tmp_value = "invalid charactor \'";
            _tmp_value.push_back(c);
            _tmp_value += "\'";
            return false; // invalid state
        }
        break;

    case State::InAttrName:
        if ('=' == c)
        {
            _state = State::ExpectFirstQuot;
            break;
        }
        if (is_space(c))
        {
            _state = State::ExpectEqual;
            break;
        }
        if (is_name_char(c))
        {
            if (should_handle_attribute())
                _tmp_name.push_back(c);
            break;
        }
        _state = State::InError;
        _tmp_value = "invalid charactor \'";
        _tmp_value.push_back(c);
        _tmp_value += "\' in attribute name";
        return false; // invalid state

    case State::ExpectEqual:
        if ('=' == c)
        {
            _state = State::ExpectFirstQuot;
            break;
        }
        if (is_space(c))
            break;
        _state = State::InError;
        _tmp_value = "expect \'=\' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::ExpectFirstQuot:
        if ('\"' == c)
        {
            _state = State::InAttrValue;
            break;
        }
        if (is_space(c))
            break;
        _state = State::InError;
        _tmp_value = "expect '\"' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::InAttrValue:
        if ('&' == c)
        {
            _tmp_encoded.push_back(c);
            _state = State::InAttrValueEncode;
            break;
        }
        if ('\"' == c)
        {
            handle_attribute();
            _state = State::ExpectSpaceBeforeAttrNameFirstChar;
            break;
        }
        if (should_handle_attribute())
            _tmp_value.push_back(c);
        break;

    case State::InAttrValueEncode:
        switch (c)
        {
        case '\"':
            handle_attribute();
            _state = State::ExpectSpaceBeforeAttrNameFirstChar;
            break;

        case '&':
            if (should_handle_attribute())
                _tmp_value += _tmp_encoded;
            _tmp_encoded.clear();
            _tmp_encoded.push_back('&');
            break;

        case ';':
            if (should_handle_attribute())
            {
                _tmp_encoded.push_back(';');
                _tmp_value += decode(_tmp_encoded);
            }
            _tmp_encoded.clear();
            _state = State::InAttrValue;
            break;

        default:
            _tmp_encoded.push_back(c);
            if (_tmp_encoded.length() >= MAX_ENCODED_LEN)
            {
                if (should_handle_attribute())
                    _tmp_value += _tmp_encoded;
                _tmp_encoded.clear();
                _state = State::InAttrValue;
            }
            break;
        }
        break;

    case State::ExpectImmediatGreaterSignAndFinishElem:
        if ('>' == c)
        {
            handle_finish();
            _state = State::InText;
            break;
        }
        _state = State::InError;
        _tmp_value = "expect \'>\' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::ExpectFirstBar:
        if ('-' == c)
        {
            _state = State::ExpectSecondBar;
            break;
        }
        _state = State::InError;
        _tmp_value = "expect '-' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::ExpectSecondBar:
        if ('-' == c)
        {
            _state = State::InComment;
            break;
        }
        _state = State::InError;
        _tmp_value = "expect '-' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::InComment:
        if ('-' == c)
        {
            _state = State::ExpectLastBar;
            break;
        }
        if (should_handle_comment())
            _tmp_value.push_back(c);
        break;

    case State::ExpectLastBar:
        if ('-' == c)
        {
            _state = State::ExpectGreaterSignAndFinishComment;
            break;
        }
        if (should_handle_comment())
        {
            _tmp_value.push_back('-');
            _tmp_value.push_back(c);
        }
        _state = State::InComment;
        break;

    case State::ExpectGreaterSignAndFinishComment:
        switch (c)
        {
        case '-':
            if (should_handle_comment())
                _tmp_value.push_back('-');
            break;

        case '>':
            handle_comment();
            _state = State::InText;
            break;

        default:
            if (should_handle_comment())
            {
                _tmp_value.push_back('-');
                _tmp_value.push_back('-');
                _tmp_value.push_back(c);
                _state = State::InComment;
            }
            break;
        }
        break;

    case State::ExpectElemNameFirstCharAndFinishElem:
        if (is_space(c))
            break;
        if (is_name_char(c))
        {
            _tmp_name.push_back(c);
            _state = State::InElemNameAndFinishElem;
            break;
        }
        _state = State::InError;
        _tmp_value = "invalid character \'";
        _tmp_value.push_back(c);
        _tmp_value += "\' for start of element name";
        return false; // invalid state

    case State::InElemNameAndFinishElem:
        if ('>' == c)
        {
            if (!check_finish())
            {
                _state = State::InError;
                _tmp_value = "finish of element \"";
                _tmp_value += _tmp_name;
                _tmp_value += "\" unmatched";
                return false;
            }
            handle_finish();
            _state = State::InText;
            break;
        }
        if (is_space(c))
        {
            if (!check_finish())
            {
                _state = State::InError;
                _tmp_value = "finish of element \"";
                _tmp_value += _tmp_name;
                _tmp_value += "\" unmatched";
                return false;
            }
            _state = State::ExpectGreaterSignAndFinishElem;
            break;
        }
        if (is_name_char(c))
        {
            _tmp_name.push_back(c);
            break;
        }
        _state = State::InError;
        _tmp_value = "invalid character \'";
        _tmp_value.push_back(c);
        _tmp_value += "\' in element name";
        return false; // invalid state

    case State::ExpectGreaterSignAndFinishElem:
        if ('>' == c)
        {
            handle_finish();
            _state = State::InText;
            break;
        }
        if (is_space(c))
            break;
        _state = State::InError;
        _tmp_value = "expect \'>\' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::InError:
        return false;

    default:
        assert(false);
        break;
    }

    // 成功则更新行列号
    if ('\n' == c)
    {
        ++_line;
        _column = 1;
    }
    else
    {
        ++_column;
    }
    return true;
}

bool XmlParser::should_handle_child() const noexcept
{
    if (_elem_path.empty())
        return false;
    const ElementInfo& current_elem = _elem_path.at(_elem_path.size() - 1);
    if (nullptr == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->get_handle_mask() & XmlElementHandler::HANDLE_CHILD);
}

bool XmlParser::should_handle_attribute() const noexcept
{
    if (_elem_path.empty())
        return false;
    const ElementInfo& current_elem = _elem_path.at(_elem_path.size() - 1);
    if (nullptr == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->get_handle_mask() & XmlElementHandler::HANDLE_ATTRIBUTE);
}

bool XmlParser::should_handle_text() const noexcept
{
    if (_elem_path.empty())
        return false;
    const ElementInfo& current_elem = _elem_path.at(_elem_path.size() - 1);
    if (nullptr == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->get_handle_mask() & XmlElementHandler::HANDLE_TEXT);
}

bool XmlParser::should_handle_comment() const noexcept
{
    if (_elem_path.empty())
        return false;
    const ElementInfo& current_elem = _elem_path.at(_elem_path.size() - 1);
    if (nullptr == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->get_handle_mask() & XmlElementHandler::HANDLE_COMMENT);
}

void XmlParser::handle_child() noexcept
{
    // Create child handler
    XmlElementHandler *child = nullptr;
    if (should_handle_child())
        child = _elem_path.at(_elem_path.size() - 1).handler->handle_child(_tmp_name);

    // Set busy
    if (nullptr != child)
    {
        assert(!child->is_busy()); // Should not be used this moment
        child->set_name(_tmp_name);
        child->set_busy(true);
    }

    // Push stack
    _elem_path.emplace_back(_tmp_name, child);
    _tmp_name.clear();
}

void XmlParser::handle_attribute() noexcept
{
    if (should_handle_attribute())
    {
        if (!_tmp_encoded.empty())
            _tmp_value += _tmp_encoded;
        _elem_path.at(_elem_path.size() - 1).handler->handle_attribute(_tmp_name, _tmp_value);
    }
    _tmp_name.clear();
    _tmp_value.clear();
    _tmp_encoded.clear();
}

void XmlParser::handle_text() noexcept
{
    if (should_handle_text())
    {
        if (!_tmp_encoded.empty())
            _tmp_value += _tmp_encoded;
        if (!_tmp_value.empty())
            _elem_path.at(_elem_path.size() - 1).handler->handle_text(_tmp_value);
    }
    _tmp_value.clear();
    _tmp_encoded.clear();
}

void XmlParser::handle_comment() noexcept
{
    if (should_handle_comment())
        _elem_path.at(_elem_path.size() - 1).handler->handle_comment(_tmp_value);
    _tmp_value.clear();
}

void XmlParser::handle_finish() noexcept
{
    if (_elem_path.empty())
        return;

    // Pop stack
    XmlElementHandler *handler = _elem_path.at(_elem_path.size() - 1).handler;
    if (nullptr != handler)
        handler->handle_finish();
    _elem_path.pop_back();

    // Set not-busy
    if (nullptr != handler)
    {
        assert(handler->is_busy());
        handler->set_busy(false);
    }

    // Notify it's parent
    if (_elem_path.empty())
        return;
    XmlElementHandler *parent = _elem_path.at(_elem_path.size() - 1).handler;
    if (nullptr != parent)
        parent->handle_child_finish(handler);
}

bool XmlParser::check_finish() noexcept
{
    if (_elem_path.empty())
        return false;
    if (_tmp_name != _elem_path.at(_elem_path.size() - 1).name)
        return false;
    _tmp_name.clear();
    return true;
}

void XmlParser::force_finish() noexcept
{
    // 强制回收资源
    while (_elem_path.size() > 1)
        handle_finish();
}

/**
 * NODE: 根 handler 的删除操作需要外部自己管理
 */
XmlParser::XmlParser(XmlElementHandler *root_handler) noexcept
    : _state(State::InText)
{
    _elem_path.emplace_back("", root_handler);
}

void XmlParser::reset(XmlElementHandler *root_handler) noexcept
{
    force_finish();
    _elem_path.clear();
    _elem_path.emplace_back("", root_handler);
    _line = 1;
    _column = 1;
    _state = State::InText;
    _tmp_name.clear();
    _tmp_value.clear();
    _tmp_encoded.clear();
}

bool XmlParser::input(const char *s, int len) noexcept
{
    assert(nullptr != s);
    for (int i = 0; (len < 0 || i < len) && '\0' != s[i]; ++i)
    {
        const bool rs = input(s[i]);
        if (!rs)
        {
            force_finish();
            return false;
        }
    }
    return true;
}

bool XmlParser::finish() noexcept
{
    if (State::InError == _state)
    {
        force_finish();
        return false;
    }

    if (0 == _elem_path.size())
    {
        _tmp_value = "there are more context after root element";
        _state = State::InError;
        return false;
    }
    else if (_elem_path.size() > 1)
    {
        _tmp_value = "element \"";
        _tmp_value += _elem_path.at(_elem_path.size() - 1).name;
        _tmp_value += "\" is not closed";
        _state = State::InError;
        force_finish();
        return false;
    }
    return true;
}

size_t XmlParser::line() const noexcept
{
    return _line;
}

size_t XmlParser::column() const noexcept
{
    return _column;
}

bool XmlParser::has_error() const noexcept
{
    return State::InError == _state;
}

std::string XmlParser::error_message() const noexcept
{
    if (!has_error())
        return "";
    return _tmp_value;
}

}
