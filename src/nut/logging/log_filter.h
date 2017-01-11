
#ifndef ___HEADFILE_96AEB548_0516_4970_A913_AF51AAC6E02C_
#define ___HEADFILE_96AEB548_0516_4970_A913_AF51AAC6E02C_

#include "../nut_config.h"
#include "log_level.h"


namespace nut
{

/**
 * 日志筛选器
 */
class NUT_API LogFilter
{
    typedef unsigned hashcode_type;

    // 字典树节点
    class NUT_API Node
    {
    public:
        ll_mask_type forbid_mask = 0;

        Node *parent = nullptr;
        hashcode_type *children_hash = nullptr; // 升序排列
        Node **children = nullptr;
        int children_size = 0, children_cap = 0;

    private:
        // Non-copyable
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

    public:
        explicit Node(Node *p);
        ~Node();

        void swap(Node *x);

        /**
         * @return >=0, 找到的位置
         *         <0, 插入位置
         */
        int search(hashcode_type hash) const;

        void ensure_cap(int new_size);

        /**
         * @param pos 必须小于 0
         */
        void insert(int pos, hashcode_type hash);

        void remove(Node *child);

        void clear();
    };
    Node _root;

private:
    // Non-copyable
    LogFilter(const LogFilter&) = delete;
    LogFilter& operator=(const LogFilter&) = delete;

    /**
     * 哈稀字符串，直到遇到结尾或者 '.' 字符
     */
    static hashcode_type hash_to_dot(const char *s, int *char_accum);

public:
    LogFilter();

    void swap(LogFilter *x);

    /**
     * 禁用指定 tag
     *
     * @param mask 禁用的 LogLevel 掩码
     */
    void forbid(const char *tag, ll_mask_type mask = LL_ALL_MASK);

    /**
     * 解禁指定 tag
     *
     * @param mask 禁用的 LogLevel 掩码
     */
    void unforbid(const char *tag, ll_mask_type mask = LL_ALL_MASK);

    void clear_forbids();

    /**
     * 查询是否被禁用掉
     */
    bool is_forbidden(const char *tag, LogLevel level) const;
};

}

#endif
