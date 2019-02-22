
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <new>

#include <nut/util/string/to_string.h>

#include "log_filter.h"


namespace nut
{

LogFilter::Node::Node(hashcode_type h, Node *p)
    : hash(h), parent(p)
{}

LogFilter::Node::~Node()
{
    clear();
}

void LogFilter::Node::swap(Node *x)
{
    assert(nullptr != x);
    if (this == x)
        return;

    loglevel_mask_type levels = allowed_levels;
    allowed_levels = x->allowed_levels;
    x->allowed_levels = levels;

    levels = forbidden_levels;
    forbidden_levels = x->forbidden_levels;
    x->forbidden_levels = levels;

    const hashcode_type h = hash;
    const_cast<hashcode_type&>(hash) = x->hash;
    const_cast<hashcode_type&>(x->hash) = h;

    Node **const chdr = children;
    children = x->children;
    x->children = chdr;

    size_t v = children_size;
    children_size = x->children_size;
    x->children_size = v;

    v = children_capacity;
    children_capacity = x->children_capacity;
    x->children_capacity = v;

    for (size_t i = 0; i < children_size; ++i)
        children[i]->parent = this;
    for (size_t i = 0; i < x->children_size; ++i)
        x->children[i]->parent = x;
}

ssize_t LogFilter::Node::search_child(hashcode_type h) const
{
    // binary search
    ssize_t left = -1, right = children_size;
    while (left + 1 < right)
    {
        const ssize_t mid = (left + right) / 2;
        if (h == children[mid]->hash)
            return mid;
        else if (h < children[mid]->hash)
            right = mid;
        else
            left = mid;
    }
    return -right - 1;
}

LogFilter::Node* LogFilter::Node::insert_child(ssize_t pos, hashcode_type h)
{
    assert(pos < 0);
    pos = -pos - 1;
    ensure_cap(children_size + 1);
    if (((size_t) pos) < children_size)
    {
        const size_t count = children_size - pos;
        ::memmove(children + pos + 1, children + pos, sizeof(Node*) * count);
    }
    Node *child = (Node*) ::malloc(sizeof(Node));
    assert(nullptr != child);
    new (child) Node(h, this);
    children[pos] = child;
    ++children_size;
    return children[pos];
}

void LogFilter::Node::remove_child(Node *child)
{
    assert(nullptr != child);
    size_t pos = 0;
    while (pos < children_size && child != children[pos])
        ++pos;
    assert(pos < children_size);

    children[pos]->~Node();
    ::free(children[pos]);

    if (pos < children_size - 1)
    {
        const size_t count = children_size - pos - 1;
        ::memmove(children + pos, children + pos + 1, sizeof(Node*) * count);
    }
    --children_size;
}

void LogFilter::Node::clear()
{
    allowed_levels = 0;
    forbidden_levels = 0;

    for (size_t i = 0; i < children_size; ++i)
    {
        assert(nullptr != children && nullptr != children[i]);
        children[i]->~Node();
        ::free(children[i]);
    }

    if (nullptr != children)
        ::free(children);
    children = nullptr;
    children_size = 0;
    children_capacity = 0;
}

void LogFilter::Node::ensure_cap(size_t new_size)
{
    if (new_size <= children_capacity)
        return;

    size_t new_cap = children_capacity * 3 / 2;
    if (new_cap < new_size)
        new_cap = new_size;

    children = (Node**) ::realloc(children, sizeof(Node*) * new_cap);
    children_capacity = new_cap;
}

static std::string levels_to_string(loglevel_mask_type levels)
{
    std::string ret;
    if (0 != (levels & LL_DEBUG))
        ret.push_back(log_level_to_char(LL_DEBUG));
    if (0 != (levels & LL_INFO))
        ret.push_back(log_level_to_char(LL_INFO));
    if (0 != (levels & LL_WARN))
        ret.push_back(log_level_to_char(LL_WARN));
    if (0 != (levels & LL_ERROR))
        ret.push_back(log_level_to_char(LL_ERROR));
    if (0 != (levels & LL_FATAL))
        ret.push_back(log_level_to_char(LL_FATAL));
    return ret;
}

std::string LogFilter::Node::to_string(const std::string& tag_prefix) const
{
    std::string tag;
    if (nullptr == parent)
        tag = "*";
    else if (tag_prefix.empty())
        tag = ullong_to_str(hash);
    else
        tag = tag_prefix + "." + ullong_to_str(hash);

    std::string ret;
    if (nullptr != parent && 0 != allowed_levels)
    {
        ret += "+ ";
        ret += tag;
        ret.push_back(' ');
        ret += levels_to_string(allowed_levels);
    }

    if (0 != forbidden_levels)
    {
        if (!ret.empty())
            ret.push_back('\n');

        ret += "- ";
        ret += tag;
        ret.push_back(' ');
        ret += levels_to_string(forbidden_levels);
    }

    if (0 == allowed_levels && 0 == forbidden_levels &&
        nullptr != parent && 0 == children_size)
    {
        // useless node, should be deleted
        ret += "? ";
        ret += tag;
    }

    std::string children_tag_prefix;
    if (nullptr != parent)
        children_tag_prefix = tag;
    for (size_t i = 0; i < children_size; ++i)
    {
        std::string s = children[i]->to_string(children_tag_prefix);
        if (!ret.empty())
            ret.push_back('\n');
        ret += s;
    }

    return ret;
}

LogFilter::LogFilter()
    : _root(hash_to_dot(nullptr), nullptr)
{}

void LogFilter::swap(LogFilter *x)
{
    assert(nullptr != x);
    if (this == x)
        return;

    _root.swap(&x->_root);
}

LogFilter::hashcode_type LogFilter::hash_to_dot(const char *s, size_t *char_accum)
{
    // SDBRHash 算法
    hashcode_type hash = 17;
    size_t i = 0;
    while (nullptr != s && 0 != s[i] && '.' != s[i])
    {
        // hash = 65599 * hash + (*s++);
        hash = s[i] + (hash << 6) + (hash << 16) - hash;
        ++i;
    }
    if (nullptr != char_accum)
        *char_accum += i;
    return hash;
}

void LogFilter::remove_empty_leaves_upway(Node *leaf)
{
    assert(nullptr != leaf);

    Node *current = leaf;
    while (0 == current->allowed_levels && 0 == current->forbidden_levels &&
        0 == current->children_size && nullptr != current->parent)
    {
        Node *parent = current->parent;
        parent->remove_child(current);
        current = parent;
    }
}

LogFilter::Node* LogFilter::find_or_create_node(const char *tag)
{
    // root
    if (nullptr == tag || 0 == tag[0])
        return &_root;

    // find or craete the node
    Node *current = &_root;
    size_t i = 0;
    do
    {
        // hash 一段标志符
        const hashcode_type hash = hash_to_dot(tag + i, &i);

        // 找到对应的节点
        assert(nullptr != current);
        const ssize_t pos = current->search_child(hash);
        if (pos < 0)
            current = current->insert_child(pos, hash);
        else
            current = current->children[pos];

        if (0 != tag[i])
        {
            assert('.' == tag[i]);
            ++i;
        }
    } while (0 != tag[i]);
    return current;
}

const LogFilter::Node* LogFilter::find_ancestor(const char *tag) const
{
    // root
    if (nullptr == tag || 0 == tag[0])
        return &_root;

    // find
    const Node *current = &_root;
    size_t i = 0;
    do
    {
        // hash 一段标志符
        const hashcode_type hash = hash_to_dot(tag + i, &i);

        // 找到对应的节点
        assert(nullptr != current);
        const ssize_t pos = current->search_child(hash);
        if (pos < 0)
            return current;
        current = current->children[pos];

        if (0 != tag[i])
        {
            assert('.' == tag[i]);
            ++i;
        }
    } while (0 != tag[i]);
    return current;
}

void LogFilter::allow(const char *tag, loglevel_mask_type levels)
{
    // dummy operation
    if (0 == levels)
        return;

    // apply the rule
    Node *n = find_or_create_node(tag);
    assert(nullptr != n);
    n->allowed_levels |= levels;
    n->forbidden_levels &= ~levels;

    // remove empty leaves
    remove_empty_leaves_upway(n);
}

void LogFilter::forbid(const char *tag, loglevel_mask_type levels)
{
    // dummy operation
    if (0 == levels)
        return;

    // apply the rule
    Node *n = find_or_create_node(tag);
    assert(nullptr != n);
    n->allowed_levels &= ~levels;
    n->forbidden_levels |= levels;

    // remove empty leaves
    remove_empty_leaves_upway(n);
}

void LogFilter::reset()
{
    _root.clear();
}

bool LogFilter::is_allowed(const char *tag, enum LogLevel level) const
{
    const Node *n = find_ancestor(tag);
    while (nullptr != n)
    {
        if (0 != (n->allowed_levels & level))
            return true;
        else if (0 != (n->forbidden_levels & level))
            return false;
        n = n->parent;
    }
    return true;
}

std::string LogFilter::to_string() const
{
    return _root.to_string("");
}

}
