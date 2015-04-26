
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <new>

#include "log_filter.h"

namespace nut
{

LogFilter::Node::Node(Node *p)
    : forbid_mask(0), parent(p), children_hash(NULL), children(NULL),
      children_size(0), children_cap(0)
{}

LogFilter::Node::~Node()
{
    clear();
}

int LogFilter::Node::search(hash_t hash) const
{
    // binary search
    int left = -1, right = children_size;
    while (left + 1 < right)
    {
        const int mid = (left + right) / 2;
        if (hash == children_hash[mid])
            return mid;
        else if (hash < children_hash[mid])
            right = mid;
        else
            left = mid;
    }
    return -right - 1;
}

void LogFilter::Node::ensure_cap(int new_size)
{
    if (new_size < children_cap)
        return;

    int new_cap = children_cap * 3 / 2;
    if (new_cap < new_size)
        new_cap = new_size;

    if (NULL == children_hash)
    {
        assert(NULL == children && 0 == children_size);
        children_hash = (hash_t*) ::malloc(sizeof(hash_t) * new_cap);
        children = (Node**) ::malloc(sizeof(Node*) * new_cap);
    }
    else
    {
        assert(NULL != children);
        children_hash = (hash_t*) ::realloc(children_hash, sizeof(hash_t) * new_cap);
        children = (Node**) ::realloc(children, sizeof(Node*) * new_cap);
    }
    children_cap = new_cap;
}

void LogFilter::Node::insert(int pos, hash_t hash)
{
    assert(pos < 0);
    pos = -pos - 1;
    ensure_cap(children_size + 1);
    if (pos < children_size)
    {
        const int count = children_size - pos;
        ::memmove(children_hash + pos + 1, children_hash + pos, sizeof(hash_t) * count);
        ::memmove(children + pos + 1, children + pos, sizeof(Node*) * count);
    }
    children_hash[pos] = hash;
    Node *child = (Node*) ::malloc(sizeof(Node));
    assert(NULL != child);
    new (child) Node(this);
    children[pos] = child;
    ++children_size;
}

void LogFilter::Node::remove(Node *child)
{
    assert(NULL != child);
    int pos = 0;
    while (pos < children_size && child != children[pos])
        ++pos;
    assert(pos < children_size);

    children[pos]->~Node();
    if (pos < children_size - 1)
    {
        const int count = children_size - pos - 1;
        ::memmove(children_hash + pos, children_hash + pos + 1, sizeof(hash_t) * count);
        ::memmove(children + pos, children + pos + 1, sizeof(Node*) * count);
    }
    --children_size;
}

void LogFilter::Node::clear()
{
    for (int i = 0; i < children_size; ++i)
    {
        assert(NULL != children && NULL != children[i]);
        children[i]->~Node();
    }

    if (NULL != children_hash)
    {
        ::free(children_hash);
        assert(NULL != children);
        ::free(children);
    }

    forbid_mask = 0;
    children_hash = NULL;
    children = NULL;
    children_size = 0;
    children_cap = 0;
}

LogFilter::LogFilter()
    : m_root(NULL)
{}

LogFilter::hash_t LogFilter::hash_to_dot(const char *s, int *char_accum)
{
    assert(NULL != s && NULL != char_accum);

    // SDBRHash 算法
    hash_t hash = 17;
    int i = 0;
    while (0 != s[i] && '.' != s[i])
    {
        // hash = 65599 * hash + (*s++);
        hash = s[i] + (hash << 6) + (hash << 16) - hash;
        ++i;
    }
    *char_accum += i;
    return hash;
}

void LogFilter::forbid(const char *tag, ll_mask_t mask)
{
    // dummy operation
    if (0 == mask)
        return;

    // root rule
    if (NULL == tag || 0 == tag[0])
    {
        m_root.forbid_mask |= mask;
        return;
    }

    // find the node
    Node *current = &m_root;
    int i = 0;
    do
    {
        // hash 一段标志符
        const hash_t hash = hash_to_dot(tag + i, &i);

        // 找到对应的节点
        assert(NULL != current);
        const int pos = current->search(hash);
        if (pos < 0)
        {
            current->insert(pos, hash);
            current = current->children[-pos - 1];
        }
        else
        {
            current = current->children[pos];
        }

        if (0 != tag[i])
        {
            assert('.' == tag[i]);
            ++i;
        }
    } while (0 != tag[i]);

    // apply the rule
    assert(NULL != current);
    current->forbid_mask |= mask;
}

void LogFilter::unforbid(const char *tag, ll_mask_t mask)
{
    // dummy operation
    if (0 == mask)
        return;

    // root rule
    if (NULL == tag || 0 == tag[0])
    {
        m_root.forbid_mask &= ~mask;
        return;
    }

    // find the node
    Node *current = &m_root;
    int i = 0;
    do
    {
        // hash 一段标志符
        const hash_t hash = hash_to_dot(tag + i, &i);

        // 找到对应的节点
        assert(NULL != current);
        const int pos = current->search(hash);
        if (pos < 0)
            return;
        else
            current = current->children[pos];

        if (0 != tag[i])
        {
            assert('.' == tag[i]);
            ++i;
        }
    } while (0 != tag[i]);

    // apply the rule
    assert(NULL != current);
    current->forbid_mask &= ~mask;

    // remove empty nodes
    while (0 == current->forbid_mask && 0 == current->children_size &&
           NULL != current->parent)
    {
        Node *parent = current->parent;
        parent->remove(current);
        current = parent;
    }
}

void LogFilter::clear_forbids()
{
    m_root.clear();
}

bool LogFilter::is_forbidden(const char *tag, LogLevel level) const
{
    // root rule
    if (0 != (m_root.forbid_mask & level))
        return true;
    if (NULL == tag || 0 == tag[0])
        return false;

    // find the node
    const Node *current = &m_root;
    int i = 0;
    do
    {
        // hash 一段标志符
        const hash_t hash = hash_to_dot(tag + i, &i);

        // 找到对应的节点
        assert(NULL != current);
        const int pos = current->search(hash);
        if (pos < 0)
            return false;
        else
            current = current->children[pos];

        // apply rule
        assert(NULL != current);
        if (0 != (current->forbid_mask & level))
            return true;

        if (0 != tag[i])
        {
            assert('.' == tag[i]);
            ++i;
        }
    } while (0 != tag[i]);

    return false;
}

}
