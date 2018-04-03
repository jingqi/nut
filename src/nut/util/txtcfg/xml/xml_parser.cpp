
#include <assert.h>

#include "xml_parser.h"

// 转义字符最大长度
#define MAX_ENCODED_LEN 6

namespace nut
{

static void decode_append(const std::string& e, std::string *appended)
{
    assert(nullptr != appended);
    if (e == "&amp;")
        appended->push_back('&');
    else if (e == "&quot;")
        appended->push_back('\"');
    else if (e == "&lt;")
        appended->push_back('<');
    else if (e == "&gt;")
        appended->push_back('>');
    else
        *appended += e; // decode failed
}

static bool is_space(char c)
{
    return ' ' == c || '\t' == c || '\r' == c || '\n' == c;
}

static bool is_name_char(char c)
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

bool XmlParser::input(char c)
{
    switch (_state)
    {
    case State::IN_TEXT:
        switch (c)
        {
        case '<':
            handle_text();
            _state = State::JUST_AFTER_LESS_SIGN;
            break;

        case '&':
            _tmp_encoded.push_back('&');
            _state = State::IN_TEXT_ENCODE;
            break;

        default:
            if (should_handle_text())
                _tmp_value.push_back(c);
            break;
        }
        break;

    case State::IN_TEXT_ENCODE:
        switch (c)
        {
        case '<':
            handle_text();
            _state = State::JUST_AFTER_LESS_SIGN;
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
                decode_append(_tmp_encoded, &_tmp_value);
            }
            _tmp_encoded.clear();
            _state = State::IN_TEXT;
            break;

        default:
            _tmp_encoded.push_back(c);
            if (_tmp_encoded.length() >= MAX_ENCODED_LEN)
            {
                if (should_handle_text())
                    _tmp_value += _tmp_encoded;
                _tmp_encoded.clear();
                _state = State::IN_TEXT;
            }
            break;
        }
        break;

    case State::JUST_AFTER_LESS_SIGN:
        switch (c)
        {
        case '/':
            _state = State::EXPECT_ELEM_NAME_FIRST_CHAR_AND_FINISH_ELEM;
            break;

        case '!':
            _state = State::EXPECT_FIRST_BAR;
            break;

        default:
            if (is_space(c))
            {
                _state = State::EXPECT_ELEM_NAME_FIRST_CHAR;
                break;
            }
            if (is_name_char(c))
            {
                if (should_handle_child())
                    _tmp_name.push_back(c);
                _state = State::IN_ELEM_NAME;
                break;
            }
            _state = State::IN_ERROR;
            _tmp_value = "invalid charactor \'";
            _tmp_value.push_back(c);
            _tmp_value += "\' for start of element name";
            return false; // invalid state
        }
        break;

    case State::EXPECT_ELEM_NAME_FIRST_CHAR:
        if (is_space(c))
            break;
        if (is_name_char(c))
        {
            if (should_handle_child())
                _tmp_name.push_back(c);
            _state = State::IN_ELEM_NAME;
            break;
        }
        _state = State::IN_ERROR;
        _tmp_value = "invalid charactor \'";
        _tmp_value.push_back(c);
        _tmp_value += "\' for start of element name";
        return false; // invalid state

    case State::IN_ELEM_NAME:
        switch (c)
        {
        case '/':
            handle_child();
            _state = State::EXPECT_IMMEDIAT_GREATER_SIGN_AND_FINISH_ELEM;
            break;

        case '>':
            handle_child();
            _state = State::IN_TEXT;
            break;

        default:
            if (is_space(c))
            {
                handle_child();
                _state = State::EXPECT_ATTR_NAME_FIRST_CHAR;
                break;
            }
            if (is_name_char(c))
            {
                if (should_handle_child())
                    _tmp_name.push_back(c);
                break;
            }
            _state = State::IN_ERROR;
            _tmp_value = "invalid charactor \'";
            _tmp_value.push_back(c);
            _tmp_value += "\' in element name";
            return false; // invalid state
        }
        break;

    case State::EXPECT_SPACE_BEFORE_ATTR_NAME_FIRST_CHAR:
        switch (c)
        {
        case '/':
            _state = State::EXPECT_IMMEDIAT_GREATER_SIGN_AND_FINISH_ELEM;
            break;

        case '>':
            _state = State::IN_TEXT;
            break;

        default:
            if (is_space(c))
            {
                _state = State::EXPECT_ATTR_NAME_FIRST_CHAR;
                break;
            }
            _state = State::IN_ERROR;
            _tmp_value = "expect SPACE but got a \'";
            _tmp_value.push_back(c);
            _tmp_value += "\'";
            return false; // invalid state
        }
        break;

    case State::EXPECT_ATTR_NAME_FIRST_CHAR:
        switch (c)
        {
        case '/':
            _state = State::EXPECT_IMMEDIAT_GREATER_SIGN_AND_FINISH_ELEM;
            break;

        case '>':
            _state = State::IN_TEXT;
            break;

        default:
            if (is_space(c))
                break;
            if (is_name_char(c))
            {
                if (should_handle_attribute())
                    _tmp_name.push_back(c);
                _state = State::IN_ATTR_NAME;
                break;
            }
            _state = State::IN_ERROR;
            _tmp_value = "invalid charactor \'";
            _tmp_value.push_back(c);
            _tmp_value += "\'";
            return false; // invalid state
        }
        break;

    case State::IN_ATTR_NAME:
        if ('=' == c)
        {
            _state = State::EXPECT_FIRST_QUOT;
            break;
        }
        if (is_space(c))
        {
            _state = State::EXPECT_EQUAL;
            break;
        }
        if (is_name_char(c))
        {
            if (should_handle_attribute())
                _tmp_name.push_back(c);
            break;
        }
        _state = State::IN_ERROR;
        _tmp_value = "invalid charactor \'";
        _tmp_value.push_back(c);
        _tmp_value += "\' in attribute name";
        return false; // invalid state

    case State::EXPECT_EQUAL:
        if ('=' == c)
        {
            _state = State::EXPECT_FIRST_QUOT;
            break;
        }
        if (is_space(c))
            break;
        _state = State::IN_ERROR;
        _tmp_value = "expect \'=\' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::EXPECT_FIRST_QUOT:
        if ('\"' == c)
        {
            _state = State::IN_ATTR_VALUE;
            break;
        }
        if (is_space(c))
            break;
        _state = State::IN_ERROR;
        _tmp_value = "expect '\"' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::IN_ATTR_VALUE:
        if ('&' == c)
        {
            _tmp_encoded.push_back(c);
            _state = State::IN_ATTR_VALUE_ENCODE;
            break;
        }
        if ('\"' == c)
        {
            handle_attribute();
            _state = State::EXPECT_SPACE_BEFORE_ATTR_NAME_FIRST_CHAR;
            break;
        }
        if (should_handle_attribute())
            _tmp_value.push_back(c);
        break;

    case State::IN_ATTR_VALUE_ENCODE:
        switch (c)
        {
        case '\"':
            handle_attribute();
            _state = State::EXPECT_SPACE_BEFORE_ATTR_NAME_FIRST_CHAR;
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
                decode_append(_tmp_encoded, &_tmp_value);
            }
            _tmp_encoded.clear();
            _state = State::IN_ATTR_VALUE;
            break;

        default:
            _tmp_encoded.push_back(c);
            if (_tmp_encoded.length() >= MAX_ENCODED_LEN)
            {
                if (should_handle_attribute())
                    _tmp_value += _tmp_encoded;
                _tmp_encoded.clear();
                _state = State::IN_ATTR_VALUE;
            }
            break;
        }
        break;

    case State::EXPECT_IMMEDIAT_GREATER_SIGN_AND_FINISH_ELEM:
        if ('>' == c)
        {
            handle_finish();
            _state = State::IN_TEXT;
            break;
        }
        _state = State::IN_ERROR;
        _tmp_value = "expect \'>\' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::EXPECT_FIRST_BAR:
        if ('-' == c)
        {
            _state = State::EXPECT_SECOND_BAR;
            break;
        }
        _state = State::IN_ERROR;
        _tmp_value = "expect '-' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::EXPECT_SECOND_BAR:
        if ('-' == c)
        {
            _state = State::IN_COMMENT;
            break;
        }
        _state = State::IN_ERROR;
        _tmp_value = "expect '-' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::IN_COMMENT:
        if ('-' == c)
        {
            _state = State::EXPECT_LAST_BAR;
            break;
        }
        if (should_handle_comment())
            _tmp_value.push_back(c);
        break;

    case State::EXPECT_LAST_BAR:
        if ('-' == c)
        {
            _state = State::EXPECT_GREATER_SIGN_AND_FINISH_COMMENT;
            break;
        }
        if (should_handle_comment())
        {
            _tmp_value.push_back('-');
            _tmp_value.push_back(c);
        }
        _state = State::IN_COMMENT;
        break;

    case State::EXPECT_GREATER_SIGN_AND_FINISH_COMMENT:
        switch (c)
        {
        case '-':
            if (should_handle_comment())
                _tmp_value.push_back('-');
            break;

        case '>':
            handle_comment();
            _state = State::IN_TEXT;
            break;

        default:
            if (should_handle_comment())
            {
                _tmp_value.push_back('-');
                _tmp_value.push_back('-');
                _tmp_value.push_back(c);
                _state = State::IN_COMMENT;
            }
            break;
        }
        break;

    case State::EXPECT_ELEM_NAME_FIRST_CHAR_AND_FINISH_ELEM:
        if (is_space(c))
            break;
        if (is_name_char(c))
        {
            _tmp_name.push_back(c);
            _state = State::IN_ELEM_NAME_AND_FINISH_ELEM;
            break;
        }
        _state = State::IN_ERROR;
        _tmp_value = "invalid character \'";
        _tmp_value.push_back(c);
        _tmp_value += "\' for start of element name";
        return false; // invalid state

    case State::IN_ELEM_NAME_AND_FINISH_ELEM:
        if ('>' == c)
        {
            if (!check_finish())
            {
                _state = State::IN_ERROR;
                _tmp_value = "finish of element \"";
                _tmp_value += _tmp_name;
                _tmp_value += "\" unmatched";
                return false;
            }
            handle_finish();
            _state = State::IN_TEXT;
            break;
        }
        if (is_space(c))
        {
            if (!check_finish())
            {
                _state = State::IN_ERROR;
                _tmp_value = "finish of element \"";
                _tmp_value += _tmp_name;
                _tmp_value += "\" unmatched";
                return false;
            }
            _state = State::EXPECT_GREATER_SIGN_AND_FINISH_ELEM;
            break;
        }
        if (is_name_char(c))
        {
            _tmp_name.push_back(c);
            break;
        }
        _state = State::IN_ERROR;
        _tmp_value = "invalid character \'";
        _tmp_value.push_back(c);
        _tmp_value += "\' in element name";
        return false; // invalid state

    case State::EXPECT_GREATER_SIGN_AND_FINISH_ELEM:
        if ('>' == c)
        {
            handle_finish();
            _state = State::IN_TEXT;
            break;
        }
        if (is_space(c))
            break;
        _state = State::IN_ERROR;
        _tmp_value = "expect \'>\' but got a \'";
        _tmp_value.push_back(c);
        _tmp_value += "\'";
        return false; // invalid state

    case State::IN_ERROR:
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

bool XmlParser::should_handle_child() const
{
    if (_elem_path.empty())
        return false;
    const ElementInfo& current_elem = _elem_path.at(_elem_path.size() - 1);
    if (nullptr == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->get_handle_mask() & XmlElementHandler::HANDLE_CHILD);
}

bool XmlParser::should_handle_attribute() const
{
    if (_elem_path.empty())
        return false;
    const ElementInfo& current_elem = _elem_path.at(_elem_path.size() - 1);
    if (nullptr == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->get_handle_mask() & XmlElementHandler::HANDLE_ATTRIBUTE);
}

bool XmlParser::should_handle_text() const
{
    if (_elem_path.empty())
        return false;
    const ElementInfo& current_elem = _elem_path.at(_elem_path.size() - 1);
    if (nullptr == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->get_handle_mask() & XmlElementHandler::HANDLE_TEXT);
}

bool XmlParser::should_handle_comment() const
{
    if (_elem_path.empty())
        return false;
    const ElementInfo& current_elem = _elem_path.at(_elem_path.size() - 1);
    if (nullptr == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->get_handle_mask() & XmlElementHandler::HANDLE_COMMENT);
}

void XmlParser::handle_child()
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
    _elem_path.push_back(ElementInfo(_tmp_name, child));
    _tmp_name.clear();
}

void XmlParser::handle_attribute()
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

void XmlParser::handle_text()
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

void XmlParser::handle_comment()
{
    if (should_handle_comment())
        _elem_path.at(_elem_path.size() - 1).handler->handle_comment(_tmp_value);
    _tmp_value.clear();
}

void XmlParser::handle_finish()
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

bool XmlParser::check_finish()
{
    if (_elem_path.empty())
        return false;
    if (_tmp_name != _elem_path.at(_elem_path.size() - 1).name)
        return false;
    _tmp_name.clear();
    return true;
}

void XmlParser::force_finish()
{
    // 强制回收资源
    while (_elem_path.size() > 1)
        handle_finish();
}

/**
 * NODE: 根 handler 的删除操作需要外部自己管理
 */
XmlParser::XmlParser(XmlElementHandler *root_handler)
    : _state(State::IN_TEXT)
{
    _elem_path.push_back(ElementInfo("", root_handler));
}

void XmlParser::reset(XmlElementHandler *root_handler)
{
    force_finish();
    _elem_path.clear();
    _elem_path.push_back(ElementInfo("", root_handler));
    _line = 1;
    _column = 1;
    _state = State::IN_TEXT;
    _tmp_name.clear();
    _tmp_value.clear();
    _tmp_encoded.clear();
}

bool XmlParser::input(const char *s, int len)
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

bool XmlParser::finish()
{
    if (State::IN_ERROR == _state)
    {
        force_finish();
        return false;
    }

    if (0 == _elem_path.size())
    {
        _tmp_value = "there are more context after root element";
        _state = State::IN_ERROR;
        return false;
    }
    else if (_elem_path.size() > 1)
    {
        _tmp_value = "element \"";
        _tmp_value += _elem_path.at(_elem_path.size() - 1).name;
        _tmp_value += "\" is not closed";
        _state = State::IN_ERROR;
        force_finish();
        return false;
    }
    return true;
}

bool XmlParser::has_error() const
{
    return State::IN_ERROR == _state;
}

std::string XmlParser::error_message() const
{
    if (!has_error())
        return "";
    return _tmp_value;
}

}
