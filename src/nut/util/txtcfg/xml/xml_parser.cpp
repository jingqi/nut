
#include <assert.h>

#include "xml_parser.h"

// 转义字符最大长度
#define MAX_ENCODED_LEN 6

namespace nut
{

static void decode_append(const std::string& e, std::string *appended)
{
    assert(NULL != appended);
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

namespace
{

enum State
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
};

}

bool XmlParser::input(char c)
{
    switch (m_state)
    {
    case IN_TEXT:
        switch (c)
        {
        case '<':
            handle_text();
            m_state = JUST_AFTER_LESS_SIGN;
            break;

        case '&':
            m_tmp_encoded.push_back('&');
            m_state = IN_TEXT_ENCODE;
            break;

        default:
            if (should_handle_text())
                m_tmp_value.push_back(c);
            break;
        }
        break;

    case IN_TEXT_ENCODE:
        switch (c)
        {
        case '<':
            handle_text();
            m_state = JUST_AFTER_LESS_SIGN;
            break;

        case '&':
            if (should_handle_text())
                m_tmp_value += m_tmp_encoded;
            m_tmp_encoded.clear();
            m_tmp_encoded.push_back('&');
            break;

        case ';':
            if (should_handle_text())
            {
                m_tmp_encoded.push_back(';');
                decode_append(m_tmp_encoded, &m_tmp_value);
            }
            m_tmp_encoded.clear();
            m_state = IN_TEXT;
            break;

        default:
            m_tmp_encoded.push_back(c);
            if (m_tmp_encoded.length() >= MAX_ENCODED_LEN)
            {
                if (should_handle_text())
                    m_tmp_value += m_tmp_encoded;
                m_tmp_encoded.clear();
                m_state = IN_TEXT;
            }
            break;
        }
        break;

    case JUST_AFTER_LESS_SIGN:
        switch (c)
        {
        case '/':
            m_state = EXPECT_ELEM_NAME_FIRST_CHAR_AND_FINISH_ELEM;
            break;

        case '!':
            m_state = EXPECT_FIRST_BAR;
            break;

        default:
            if (is_space(c))
            {
                m_state = EXPECT_ELEM_NAME_FIRST_CHAR;
                break;
            }
            if (is_name_char(c))
            {
                if (should_handle_child())
                    m_tmp_name.push_back(c);
                m_state = IN_ELEM_NAME;
                break;
            }
            m_state = IN_ERROR;
            m_tmp_value = "invalid charactor \'";
            m_tmp_value.push_back(c);
            m_tmp_value += "\' for start of element name";
            return false; // invalid state
        }
        break;

    case EXPECT_ELEM_NAME_FIRST_CHAR:
        if (is_space(c))
            break;
        if (is_name_char(c))
        {
            if (should_handle_child())
                m_tmp_name.push_back(c);
            m_state = IN_ELEM_NAME;
            break;
        }
        m_state = IN_ERROR;
        m_tmp_value = "invalid charactor \'";
        m_tmp_value.push_back(c);
        m_tmp_value += "\' for start of element name";
        return false; // invalid state

    case IN_ELEM_NAME:
        switch (c)
        {
        case '/':
            handle_child();
            m_state = EXPECT_IMMEDIAT_GREATER_SIGN_AND_FINISH_ELEM;
            break;

        case '>':
            handle_child();
            m_state = IN_TEXT;
            break;

        default:
            if (is_space(c))
            {
                handle_child();
                m_state = EXPECT_ATTR_NAME_FIRST_CHAR;
                break;
            }
            if (is_name_char(c))
            {
                if (should_handle_child())
                    m_tmp_name.push_back(c);
                break;
            }
            m_state = IN_ERROR;
            m_tmp_value = "invalid charactor \'";
            m_tmp_value.push_back(c);
            m_tmp_value += "\' in element name";
            return false; // invalid state
        }
        break;

    case EXPECT_SPACE_BEFORE_ATTR_NAME_FIRST_CHAR:
        switch (c)
        {
        case '/':
            m_state = EXPECT_IMMEDIAT_GREATER_SIGN_AND_FINISH_ELEM;
            break;

        case '>':
            m_state = IN_TEXT;
            break;

        default:
            if (is_space(c))
            {
                m_state = EXPECT_ATTR_NAME_FIRST_CHAR;
                break;
            }
            m_state = IN_ERROR;
            m_tmp_value = "expect SPACE but got a \'";
            m_tmp_value.push_back(c);
            m_tmp_value += "\'";
            return false; // invalid state
        }
        break;

    case EXPECT_ATTR_NAME_FIRST_CHAR:
        switch (c)
        {
        case '/':
            m_state = EXPECT_IMMEDIAT_GREATER_SIGN_AND_FINISH_ELEM;
            break;

        case '>':
            m_state = IN_TEXT;
            break;

        default:
            if (is_space(c))
                break;
            if (is_name_char(c))
            {
                if (should_handle_attribute())
                    m_tmp_name.push_back(c);
                m_state = IN_ATTR_NAME;
                break;
            }
            m_state = IN_ERROR;
            m_tmp_value = "invalid charactor \'";
            m_tmp_value.push_back(c);
            m_tmp_value += "\'";
            return false; // invalid state
        }
        break;

    case IN_ATTR_NAME:
        if ('=' == c)
        {
            m_state = EXPECT_FIRST_QUOT;
            break;
        }
        if (is_space(c))
        {
            m_state = EXPECT_EQUAL;
            break;
        }
        if (is_name_char(c))
        {
            if (should_handle_attribute())
                m_tmp_name.push_back(c);
            break;
        }
        m_state = IN_ERROR;
        m_tmp_value = "invalid charactor \'";
        m_tmp_value.push_back(c);
        m_tmp_value += "\' in attribute name";
        return false; // invalid state

    case EXPECT_EQUAL:
        if ('=' == c)
        {
            m_state = EXPECT_FIRST_QUOT;
            break;
        }
        if (is_space(c))
            break;
        m_state = IN_ERROR;
        m_tmp_value = "expect \'=\' but got a \'";
        m_tmp_value.push_back(c);
        m_tmp_value += "\'";
        return false; // invalid state

    case EXPECT_FIRST_QUOT:
        if ('\"' == c)
        {
            m_state = IN_ATTR_VALUE;
            break;
        }
        if (is_space(c))
            break;
        m_state = IN_ERROR;
        m_tmp_value = "expect '\"' but got a \'";
        m_tmp_value.push_back(c);
        m_tmp_value += "\'";
        return false; // invalid state

    case IN_ATTR_VALUE:
        if ('&' == c)
        {
            m_tmp_encoded.push_back(c);
            m_state = IN_ATTR_VALUE_ENCODE;
            break;
        }
        if ('\"' == c)
        {
            handle_attribute();
            m_state = EXPECT_SPACE_BEFORE_ATTR_NAME_FIRST_CHAR;
            break;
        }
        if (should_handle_attribute())
            m_tmp_value.push_back(c);
        break;

    case IN_ATTR_VALUE_ENCODE:
        switch (c)
        {
        case '\"':
            handle_attribute();
            m_state = EXPECT_SPACE_BEFORE_ATTR_NAME_FIRST_CHAR;
            break;

        case '&':
            if (should_handle_attribute())
                m_tmp_value += m_tmp_encoded;
            m_tmp_encoded.clear();
            m_tmp_encoded.push_back('&');
            break;

        case ';':
            if (should_handle_attribute())
            {
                m_tmp_encoded.push_back(';');
                decode_append(m_tmp_encoded, &m_tmp_value);
            }
            m_tmp_encoded.clear();
            m_state = IN_ATTR_VALUE;
            break;

        default:
            m_tmp_encoded.push_back(c);
            if (m_tmp_encoded.length() >= MAX_ENCODED_LEN)
            {
                if (should_handle_attribute())
                    m_tmp_value += m_tmp_encoded;
                m_tmp_encoded.clear();
                m_state = IN_ATTR_VALUE;
            }
            break;
        }
        break;

    case EXPECT_IMMEDIAT_GREATER_SIGN_AND_FINISH_ELEM:
        if ('>' == c)
        {
            handle_finish();
            m_state = IN_TEXT;
            break;
        }
        m_state = IN_ERROR;
        m_tmp_value = "expect \'>\' but got a \'";
        m_tmp_value.push_back(c);
        m_tmp_value += "\'";
        return false; // invalid state

    case EXPECT_FIRST_BAR:
        if ('-' == c)
        {
            m_state = EXPECT_SECOND_BAR;
            break;
        }
        m_state = IN_ERROR;
        m_tmp_value = "expect '-' but got a \'";
        m_tmp_value.push_back(c);
        m_tmp_value += "\'";
        return false; // invalid state

    case EXPECT_SECOND_BAR:
        if ('-' == c)
        {
            m_state = IN_COMMENT;
            break;
        }
        m_state = IN_ERROR;
        m_tmp_value = "expect '-' but got a \'";
        m_tmp_value.push_back(c);
        m_tmp_value += "\'";
        return false; // invalid state

    case IN_COMMENT:
        if ('-' == c)
        {
            m_state = EXPECT_LAST_BAR;
            break;
        }
        if (should_handle_comment())
            m_tmp_value.push_back(c);
        break;

    case EXPECT_LAST_BAR:
        if ('-' == c)
        {
            m_state = EXPECT_GREATER_SIGN_AND_FINISH_COMMENT;
            break;
        }
        if (should_handle_comment())
        {
            m_tmp_value.push_back('-');
            m_tmp_value.push_back(c);
        }
        m_state = IN_COMMENT;
        break;

    case EXPECT_GREATER_SIGN_AND_FINISH_COMMENT:
        switch (c)
        {
        case '-':
            if (should_handle_comment())
                m_tmp_value.push_back('-');
            break;

        case '>':
            handle_comment();
            m_state = IN_TEXT;
            break;

        default:
            if (should_handle_comment())
            {
                m_tmp_value.push_back('-');
                m_tmp_value.push_back('-');
                m_tmp_value.push_back(c);
                m_state = IN_COMMENT;
            }
            break;
        }
        break;

    case EXPECT_ELEM_NAME_FIRST_CHAR_AND_FINISH_ELEM:
        if (is_space(c))
            break;
        if (is_name_char(c))
        {
            m_tmp_name.push_back(c);
            m_state = IN_ELEM_NAME_AND_FINISH_ELEM;
            break;
        }
        m_state = IN_ERROR;
        m_tmp_value = "invalid character \'";
        m_tmp_value.push_back(c);
        m_tmp_value += "\' for start of element name";
        return false; // invalid state

    case IN_ELEM_NAME_AND_FINISH_ELEM:
        if ('>' == c)
        {
            if (!check_finish())
            {
                m_state = IN_ERROR;
                m_tmp_value = "finish of element \"";
                m_tmp_value += m_tmp_name;
                m_tmp_value += "\" unmatched";
                return false;
            }
            handle_finish();
            m_state = IN_TEXT;
            break;
        }
        if (is_space(c))
        {
            if (!check_finish())
            {
                m_state = IN_ERROR;
                m_tmp_value = "finish of element \"";
                m_tmp_value += m_tmp_name;
                m_tmp_value += "\" unmatched";
                return false;
            }
            m_state = EXPECT_GREATER_SIGN_AND_FINISH_ELEM;
            break;
        }
        if (is_name_char(c))
        {
            m_tmp_name.push_back(c);
            break;
        }
        m_state = IN_ERROR;
        m_tmp_value = "invalid character \'";
        m_tmp_value.push_back(c);
        m_tmp_value += "\' in element name";
        return false; // invalid state

    case EXPECT_GREATER_SIGN_AND_FINISH_ELEM:
        if ('>' == c)
        {
            handle_finish();
            m_state = IN_TEXT;
            break;
        }
        if (is_space(c))
            break;
        m_state = IN_ERROR;
        m_tmp_value = "expect \'>\' but got a \'";
        m_tmp_value.push_back(c);
        m_tmp_value += "\'";
        return false; // invalid state

    case IN_ERROR:
        return false;

    default:
        assert(false);
        break;
    }

    // 成功则更新行列号
    if ('\n' == c)
    {
        ++m_line;
        m_column = 1;
    }
    else
    {
        ++m_column;
    }
    return true;
}

bool XmlParser::should_handle_child() const
{
    if (m_elem_path.empty())
        return false;
    const ElementInfo& current_elem = m_elem_path.at(m_elem_path.size() - 1);
    if (NULL == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->handle_mask & XmlElementHandler::HANDLE_CHILD);
}

bool XmlParser::should_handle_attribute() const
{
    if (m_elem_path.empty())
        return false;
    const ElementInfo& current_elem = m_elem_path.at(m_elem_path.size() - 1);
    if (NULL == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->handle_mask & XmlElementHandler::HANDLE_ATTRIBUTE);
}

bool XmlParser::should_handle_text() const
{
    if (m_elem_path.empty())
        return false;
    const ElementInfo& current_elem = m_elem_path.at(m_elem_path.size() - 1);
    if (NULL == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->handle_mask & XmlElementHandler::HANDLE_TEXT);
}

bool XmlParser::should_handle_comment() const
{
    if (m_elem_path.empty())
        return false;
    const ElementInfo& current_elem = m_elem_path.at(m_elem_path.size() - 1);
    if (NULL == current_elem.handler)
        return false;
    return 0 != (current_elem.handler->handle_mask & XmlElementHandler::HANDLE_COMMENT);
}

void XmlParser::handle_child()
{
    XmlElementHandler *child = NULL;
    if (should_handle_child())
        child = m_elem_path.at(m_elem_path.size() - 1).handler->handle_child(m_tmp_name);

    m_elem_path.push_back(ElementInfo(m_tmp_name, child));
    m_tmp_name.clear();
}

void XmlParser::handle_attribute()
{
    if (should_handle_attribute())
    {
        if (!m_tmp_encoded.empty())
            m_tmp_value += m_tmp_encoded;
        m_elem_path.at(m_elem_path.size() - 1).handler->handle_attribute(m_tmp_name, m_tmp_value);
    }
    m_tmp_name.clear();
    m_tmp_value.clear();
    m_tmp_encoded.clear();
}

void XmlParser::handle_text()
{
    if (should_handle_text())
    {
        if (!m_tmp_encoded.empty())
            m_tmp_value += m_tmp_encoded;
        if (!m_tmp_value.empty())
            m_elem_path.at(m_elem_path.size() - 1).handler->handle_text(m_tmp_value);
    }
    m_tmp_value.clear();
    m_tmp_encoded.clear();
}

void XmlParser::handle_comment()
{
    if (should_handle_comment())
        m_elem_path.at(m_elem_path.size() - 1).handler->handle_comment(m_tmp_value);
    m_tmp_value.clear();
}

void XmlParser::handle_finish()
{
    if (m_elem_path.empty())
        return;
    XmlElementHandler *handler = m_elem_path.at(m_elem_path.size() - 1).handler;
    if (NULL != handler)
        handler->handle_finish();
    m_elem_path.pop_back();

    if (m_elem_path.empty())
        return;
    XmlElementHandler *parent = m_elem_path.at(m_elem_path.size() - 1).handler;
    if (NULL != parent)
        parent->handle_child_finish(handler);
}

bool XmlParser::check_finish()
{
    if (m_elem_path.empty())
        return false;
    if (m_tmp_name != m_elem_path.at(m_elem_path.size() - 1).name)
        return false;
    m_tmp_name.clear();
    return true;
}

void XmlParser::force_finish()
{
    // 强制回收资源
    while (m_elem_path.size() > 1)
        handle_finish();
}

/**
 * NODE: 根 handler 的删除操作需要外部自己管理
 */
XmlParser::XmlParser(XmlElementHandler *root_handler)
    : m_line(1), m_column(1), m_state(IN_TEXT)
{
    m_elem_path.push_back(ElementInfo("", root_handler));
}

void XmlParser::reset(XmlElementHandler *root_handler)
{
    force_finish();
    m_elem_path.clear();
    m_elem_path.push_back(ElementInfo("", root_handler));
    m_line = 1;
    m_column = 1;
    m_state = IN_TEXT;
    m_tmp_name.clear();
    m_tmp_value.clear();
    m_tmp_encoded.clear();
}

bool XmlParser::input(const char *s, int len)
{
    assert(NULL != s);
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
    if (IN_ERROR == m_state)
    {
        force_finish();
        return false;
    }

    if (0 == m_elem_path.size())
    {
        m_tmp_value = "there are more context after root element";
        m_state = IN_ERROR;
        return false;
    }
    else if (m_elem_path.size() > 1)
    {
        m_tmp_value = "element \"";
        m_tmp_value += m_elem_path.at(m_elem_path.size() - 1).name;
        m_tmp_value += "\" is not closed";
        m_state = IN_ERROR;
        force_finish();
        return false;
    }
    return true;
}

bool XmlParser::has_error() const
{
    return IN_ERROR == m_state;
}

std::string XmlParser::error_message() const
{
    if (!has_error())
        return "";
    return m_tmp_value;
}

}
