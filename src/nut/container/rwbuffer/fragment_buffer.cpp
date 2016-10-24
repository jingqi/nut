﻿
#include <assert.h>
#include <stdlib.h>
#include <algorithm>

#include "fragment_buffer.h"


namespace nut
{

FragmentBuffer::FragmentBuffer()
{}

FragmentBuffer::FragmentBuffer(const FragmentBuffer& x)
{
    *this = x;
}

FragmentBuffer::~FragmentBuffer()
{
    clear();
}

FragmentBuffer& FragmentBuffer::operator=(const FragmentBuffer& x)
{
    clear();

    Fragment *p = x._read_fragment;
    while (NULL != p)
    {
        Fragment *new_frag = new_fragment(p->size);
        assert(NULL != new_frag);
        ::memcpy(new_frag->buffer, p->buffer, p->size);
        new_frag->size = p->size;

        enqueue(new_frag);
    }
    _read_index = x._read_index;
    _read_available = x._read_available;

    return *this;
}

void FragmentBuffer::enqueue(Fragment *frag)
{
    assert(NULL != frag && frag->capacity >= frag->size);

    frag->next = NULL;
    if (NULL == _write_fragment)
    {
        _read_fragment = frag;
        _write_fragment = frag;
        _read_index = 0;
    }
    else
    {
        _write_fragment->next = frag;
        _write_fragment = frag;
    }
    _read_available += frag->size;
}

void FragmentBuffer::clear()
{
    Fragment *p = _read_fragment;
    while (NULL != p)
    {
        Fragment *next = p->next;
        delete_fragment(p);
        p = next;
    }
    _read_fragment = NULL;
    _write_fragment = NULL;
    _read_index = 0;
    _read_available = 0;
}

size_t FragmentBuffer::readable_size() const
{
    return _read_available;
}

size_t FragmentBuffer::read(void *buf, size_t len)
{
    const size_t can_read = std::min(len, _read_available);
    size_t readed = 0;
    while (readed < can_read)
    {
        assert(NULL != _read_fragment && _read_fragment->size >= _read_index);
        const size_t can_read_once = std::min(_read_fragment->size - _read_index,
                                              can_read - readed);
        const bool full_read = (_read_fragment->size - _read_index <=
                                can_read - readed);
        ::memcpy((uint8_t*) buf + readed, _read_fragment->buffer + _read_index,
                 can_read_once);
        if (full_read)
        {
            Fragment *next = _read_fragment->next;
            delete_fragment(_read_fragment);
            _read_fragment = next;
            if (NULL == next)
                _write_fragment = NULL;
            _read_index = 0;
        }
        else
        {
            _read_index += can_read_once;
        }
        readed += can_read_once;
    }
    assert(readed == can_read);
    _read_available -= can_read;
    return can_read;
}

size_t FragmentBuffer::look_ahead(void *buf, size_t len) const
{
    const size_t can_read = std::min(len, _read_available);
    size_t readed = 0;
    Fragment *p = _read_fragment;
    size_t read_index = _read_index;
    while (readed < can_read)
    {
        assert(NULL != p && p->size >= read_index);
        const size_t can_read_once = std::min(p->size - read_index,
                                              can_read - readed);
        const bool full_read = (p->size - read_index <= can_read - readed);
        ::memcpy((uint8_t*) buf + readed, p->buffer + read_index,
                 can_read_once);
        if (full_read)
        {
            p = p->next;
            read_index = 0;
        }
        readed += can_read_once;
    }
    assert(readed == can_read);
    return can_read;
}

size_t FragmentBuffer::skip_read(size_t len)
{
    size_t can_skip = std::min(len, _read_available);
    size_t skiped = 0;
    while (skiped < can_skip)
    {
        assert(NULL != _read_fragment && _read_fragment->size >= _read_index);
        const size_t can_skip_once = std::min(_read_fragment->size - _read_index,
                                              can_skip - skiped);
        const bool full_skip = (_read_fragment->size - _read_index <=
                                can_skip - skiped);
        if (full_skip)
        {
            Fragment *next = _read_fragment->next;
            delete_fragment(_read_fragment);
            _read_fragment = next;
            if (NULL == next)
                _write_fragment = NULL;
            _read_index = 0;
        }
        else
        {
            _read_index += can_skip_once;
        }
        skiped += can_skip_once;
    }
    assert(skiped == can_skip);
    _read_available -= can_skip;
    return can_skip;
}

size_t FragmentBuffer::readable_pointers(const void **buf_ptrs, size_t *len_ptrs,
                                         size_t ptr_count) const
{
    assert(NULL != buf_ptrs && NULL != len_ptrs);

    size_t buf_count = 0;
    Fragment *p = _read_fragment;
    size_t read_index = _read_index;
    while (NULL != p && buf_count < ptr_count)
    {
        assert(p->size >= read_index);
        *buf_ptrs = p->buffer + read_index;
        *len_ptrs = p->size - read_index;

        ++buf_ptrs;
        ++len_ptrs;
        ++buf_count;

        p = p->next;
        read_index = 0;
    }
    return buf_count;
}

void FragmentBuffer::write(const void *buf, size_t len)
{
    assert(NULL != buf);

    if (NULL != _write_fragment &&
        _write_fragment->capacity - _write_fragment->size >= len)
    {
        ::memcpy(_write_fragment->buffer + _write_fragment->size, buf, len);
        _write_fragment->size += len;
        _read_available += len;
        return;
    }

    Fragment *frag = new_fragment(len);
    ::memcpy(frag->buffer, buf, len);
    frag->size = len;
    enqueue(frag);
}

FragmentBuffer::Fragment* FragmentBuffer::new_fragment(size_t capacity)
{
    assert(capacity > 0);
    Fragment* p = (Fragment*) ::malloc(sizeof(Fragment) + capacity - 1);
    assert(NULL != p);
    new (p) Fragment(capacity);
    p->size = 0;
    p->next = NULL;
    return p;
}

void FragmentBuffer::delete_fragment(Fragment *frag)
{
    assert(NULL != frag);
    frag->~Fragment();
    ::free(frag);
}

FragmentBuffer::Fragment* FragmentBuffer::write_fragment(Fragment *frag)
{
    assert(NULL != frag && frag->size <= frag->capacity);
    if (NULL != _write_fragment &&
        _write_fragment->capacity - _write_fragment->size >= frag->size)
    {
        ::memcpy(_write_fragment->buffer + _write_fragment->size, frag->buffer,
                 frag->size);
        _write_fragment->size += frag->size;
        _read_available += frag->size;
        frag->size = 0;
        return frag;
    }

    enqueue(frag);
    return NULL;
}

}