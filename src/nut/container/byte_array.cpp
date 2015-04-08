
#include <assert.h>
#include <string.h>  /* for memset() and memcpy() */
#include <stdlib.h>  /* for malloc() and free() */
#include <stdio.h> /* for size_t */
#include <stdint.h>
#include <string>

#include <nut/mem/sys_ma.h>

#include "byte_array.h"

namespace nut
{

/**
 * 写前复制
 */
void ByteArray::copy_on_write()
{
    assert(m_array.is_not_null());
    const int rc = m_array->get_ref();
    assert(rc >= 1);
    if (rc > 1)
        m_array = m_array->clone();
}

ByteArray::ByteArray(memory_allocator *ma)
    : m_array(RC_NEW(ma, rcarray_type, 0, ma))
{}

/**
 * @param len initial data size
 * @param fillv initial data filling
 */
ByteArray::ByteArray(size_type len, uint8_t fillv, memory_allocator *ma)
    : m_array(RC_NEW(ma, rcarray_type, len, ma))
{
    m_array->resize(len, fillv);
}

/**
 * @param buf initial data
 * @param len initial data size
 */
ByteArray::ByteArray(const void *buf, size_type len, memory_allocator *ma)
    : m_array(RC_NEW(ma, rcarray_type, len, ma))
{
    assert(NULL != buf);
    if (NULL != buf)
        m_array->insert(0, (const uint8_t*)buf, ((const uint8_t*)buf) + len);
}

/**
 * @param term_byte the initial data terminated with 'term_byte'
 * @param include_term_byte if True, the 'term_byte' is part of initial data
 */
ByteArray::ByteArray(const void *buf, unsigned char term_byte, bool include_term_byte, memory_allocator *ma)
    : m_array(NULL)
{
    assert(NULL != buf);
    if (NULL == buf)
    {
        m_array = RC_NEW(ma, rcarray_type, 0, ma);
        return;
    }

    size_type len = 0;
    while (((const uint8_t*)buf)[len] != term_byte)
        ++len;
    if (include_term_byte)
        ++len;

    m_array = RC_NEW(ma, rcarray_type, len, ma);
    m_array->insert(0, (const uint8_t*)buf, ((const uint8_t*)buf) + len);
}

ByteArray::ByteArray(const void *buf, size_type index, size_type size, memory_allocator *ma)
    : m_array(RC_NEW(ma, rcarray_type, size, ma))
{
    assert(NULL != buf);
    if (NULL != buf)
        m_array->insert(0, ((const uint8_t*)buf) + index, ((const uint8_t*)buf) + index + size);
}

ByteArray::ByteArray(const self_type& x)
    : m_array(x.m_array)
{}

/**
 * clear data
 */
void ByteArray::clear()
{
    const int rc = m_array->get_ref();
    if (rc > 1)
        m_array = RC_NEW(m_array->allocator(), rcarray_type, 0, m_array->allocator());
    else
        m_array->clear();
}

ByteArray& ByteArray::operator=(const self_type& x)
{
    m_array = x.m_array;
    return *this;
}

void ByteArray::append(const void *buf, size_t len)
{
    assert(0 == len || NULL != buf);
    if (0 == len || NULL == buf)
        return;

    copy_on_write();
    m_array->insert(size(), (const uint8_t*)buf, ((const uint8_t*)buf) + len);
}

void ByteArray::append(const void *buf, uint8_t term_byte, bool include_term_byte)
{
    assert(NULL != buf);
    if (NULL == buf)
        return;

    size_t len = 0;
    while (((const uint8_t*)buf)[len] != term_byte)
        ++len;
    if (include_term_byte)
        ++len;
    append(buf, len);
}


/**
 * 将二进制 0x51 0x5e 0x30 转换为字符串 "515e30"
 */
void ByteArray::to_string(std::string *appended, size_t from, size_t to) const
{
    assert(NULL != appended && from <= to);

    const size_t limit = (size() < to ? size() : to);
    for (size_t i = from; i < limit; ++i)
    {
        const uint8_t b = at(i);
        int n = (b >> 4) & 0xF;
        appended->push_back(n < 10 ? ('0' + n) : ('A' + (n - 10)));

        n = b & 0xF;
        appended->push_back(n < 10 ? ('0' + n) : ('A' + (n - 10)));
    }
}

void ByteArray::to_string(std::wstring *appended, size_t from, size_t to) const
{
    assert(NULL != appended && from <= to);

    const size_t limit = (size() < to ? size() : to);
    for (size_t i = from; i < limit; ++i)
    {
        const uint8_t b = at(i);
        int n = (b >> 4) & 0xF;
        appended->push_back(n < 10 ? (L'0' + n) : (L'A' + (n - 10)));

        n = b & 0xF;
        appended->push_back(n < 10 ? (L'0' + n) : (L'A' + (n - 10)));
    }
}

/**
 * 将类似于 "1234ab" 的字符串转为二进制 0x12 0x34 0xab
 * NOTE:
 *      如果限定范围的字符串长度不是偶数，最后一个字符被忽略
 *      如果字符串中间出现非16进制字符，则转换过程立即停止
 */
ByteArray ByteArray::value_of(const std::string& s, size_t from, size_t to)
{
    assert(from <= to);

    self_type ret;
    const size_t limit = (s.length() < to ? s.length() : to);
    for (size_t i = 0; from + i * 2 + 1 < limit; ++i)
    {
        int p1, p2;
        char c = s.at(from + i * 2);
        if ('0' <= c && c <= '9')
            p1 = c - '0';
        else if (('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
            p1 = (c | 0x20) - 'a' + 10;
        else
            break;

        c = s.at(from + i * 2 + 1);
        if ('0' <= c && c <= '9')
            p2 = c - '0';
        else if (('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
            p2 = (c | 0x20) - 'a' + 10;
        else
            break;

        ret.append(1, (uint8_t)(p1 * 16 + p2));
    }
    return ret;
}

ByteArray ByteArray::value_of(const std::wstring& s, size_t from, size_t to)
{
    assert(from <= to);

    self_type ret;
    const size_t limit = (s.length() < to ? s.length() : to);
    for (size_t i = 0; from + i * 2 + 1 < limit; ++i)
    {
        int p1, p2;
        wchar_t c = s.at(from + i * 2);
        if (L'0' <= c && c <= L'9')
            p1 = c - L'0';
        else if ((L'a' <= c && c <= L'f') || (L'A' <= c && c <= L'F'))
            p1 = (c & 0x20) - L'a' + 10;
        else
            break;

        c = s.at(from + i * 2 + 1);
        if (L'0' <= c && c <= L'9')
            p2 = c - L'0';
        else if ((L'a' <= c && c <= L'f') || (L'A' <= c && c <= L'F'))
            p2 = (c & 0x20) - L'a' + 10;
        else
            break;

        ret.append(1, (uint8_t)(p1 * 16 + p2));
    }
    return ret;
}

}
