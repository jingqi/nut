
#ifndef ___HEADFILE_96AEB548_0516_4970_A913_AF51AAC6E02C_
#define ___HEADFILE_96AEB548_0516_4970_A913_AF51AAC6E02C_

#include <string>

#include "../nut_config.h"
#include "../platform/int_type.h"
#include "log_level.h"


namespace nut
{

/**
 * 日志筛选器
 */
class NUT_API LogFilter
{
private:
    typedef size_t hashcode_type;

    // 字典树节点
    class NUT_API Node
    {
    public:
        explicit Node(hashcode_type h, Node *p) noexcept;
        ~Node() noexcept;

        void swap(Node *x) noexcept;

        /**
         * @return >=0, 找到的位置
         *         <0, 插入位置
         */
        ssize_t search_child(hashcode_type h) const noexcept;

        /**
         * @param pos 必须小于 0
         */
        Node* insert_child(ssize_t pos, hashcode_type h) noexcept;

        void remove_child(Node *child) noexcept;

        void clear() noexcept;

        void ensure_cap(size_t new_size) noexcept;

        std::string to_string(const std::string& tag_prefix) const noexcept;

    private:
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

    public:
        const hashcode_type hash;
        loglevel_mask_type allowed_levels = 0;
        loglevel_mask_type forbidden_levels = 0;

        Node *parent = nullptr;
        Node **children = nullptr;
        size_t children_size = 0, children_capacity = 0;
    };

public:
    LogFilter() noexcept;

    void swap(LogFilter *x) noexcept;

    /**
     * 允许指定 tag
     *
     * @param levels 禁用的 LogLevel 掩码
     */
    void allow(const char *tag, loglevel_mask_type levels = LL_ALL_LEVELS) noexcept;

    /**
     * 禁止指定 tag
     *
     * @param levels 禁用的 LogLevel 掩码
     */
    void forbid(const char *tag, loglevel_mask_type levels = LL_ALL_LEVELS) noexcept;

    /**
     * 重置
     */
    void reset() noexcept;

    /**
     * 查询是否允许
     */
    bool is_allowed(const char *tag, enum LogLevel level) const noexcept;

    std::string to_string() const noexcept;

private:
    LogFilter(const LogFilter&) = delete;
    LogFilter& operator=(const LogFilter&) = delete;

    /**
     * 哈稀字符串，直到遇到结尾或者 '.' 字符
     *
     * @parama char_accum 用于累加参与 hash 的字符数
     */
    static hashcode_type hash_to_dot(const char *s, size_t *char_accum = nullptr) noexcept;

    /**
     * 找到或者创建节点
     */
    Node* find_or_create_node(const char *tag) noexcept;

    /**
     * 找到节点或者节点的祖先
     */
    const Node* find_ancestor(const char *tag) const noexcept;

    /**
     * 删除空叶子节点
     */
    void remove_empty_leaves_upway(Node *leaf) noexcept;

private:
    Node _root;
};

}

#endif
