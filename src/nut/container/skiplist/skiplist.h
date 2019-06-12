
#ifndef ___HEADFILE_CD9958A8_95CB_49A7_A294_60B2304EDB7E_
#define ___HEADFILE_CD9958A8_95CB_49A7_A294_60B2304EDB7E_

#include <assert.h>
#include <stdlib.h>

#include "../comparable.h"


namespace nut
{

/**
 * skip-list 的公共算法
 *
 * @param K 键值类型，要求能用 "<" 操作符比较大小
 * @param NODE 节点类型，要求实现以下方法
 *      const K& get_key() const   获取键值
 *      int get_level() const      获取 0-based 层数
 *      NODE* get_next(int) const  获取指定层数的指针
 *      void set_level(int)        设置层数，并自动分配 level+1 长度的 next 数组
 *      void set_next(int,NODE*)   设置指定层数的指针
 * @param SL 跳表数据结构本身，要求实现以下方法
 *      int get_level() const      获取跳表 0-based 层数
 *      NODE* get_head(int) const  获取跳表头
 *      void set_level(int)        设置层数，并自动分配 level+1 长度的 head 数组
 *      void set_head(int,NODE*)   设置跳表头
 */
template <typename K, typename NODE, typename SL>
class SkipList
{
public:
    /* 底层节点是上层的多少倍(>1) */
    static constexpr int LEVEL_FACTOR = 2;

    /* 最大level数, 0-based */
    static constexpr int MAX_LEVEL = 16;

    /* 无效level */
    static constexpr int INVALID_LEVEL = -1;

    /**
     * 随机化 level
     *
     * @return 0-based
     */
    static int random_level() noexcept
    {
        const int r = ::rand();
        int level = 0;
        for (int f = LEVEL_FACTOR; r < RAND_MAX / f && level < MAX_LEVEL; f *= LEVEL_FACTOR)
            ++level;
        return level;
    }

    /**
     * 查找节点，附带查找各层前向节点
     *
     * @param k         要查找的键值
     * @param sl        跳表本身
     * @param pre_lv    存放返回值，可以是 nullptr. 存放前向节点的返回值数组，长度为 (level+1).
     */
    static NODE* search_node(const K& key, const SL& sl, NODE **pre_lv) noexcept
    {
        NODE *ret = nullptr;
        NODE *pre = nullptr;
        int lv = sl.get_level();
        assert(lv >= 0);
        do
        {
            while (true)
            {
                NODE *n = (nullptr == pre ? sl.get_head(lv) : pre->get_next(lv));
                if (nullptr == n)
                {
                    if (nullptr != pre_lv)
                        pre_lv[lv] = pre;
                    break;
                }

                const int rs = compare(key, n->get_key());
                if (rs < 0)
                {
                    if (nullptr != pre_lv)
                        pre_lv[lv] = pre;
                    break;
                }
                else if (rs > 0)
                {
                    pre = n;
                }
                else
                {
                    if (nullptr == pre_lv)
                    {
                        return n; // 找到节点直接返回
                    }
                    else
                    {
                        pre_lv[lv] = pre;
                        ret = n;
                        break; // 即使找到，也继续找，以便填充完 pre_lv[]
                    }
                }
            }
        } while (--lv >= 0);
        return ret;
    }

    /**
     * 插入节点
     *
     * @param n         要插入的节点
     *                  如果其leve数无效，则会生成随机level数，否则使用原有的level数
     * @param sl        跳表本身
     * @param pre_lv    前向节点数组，长度为 (level+1)
     */
    static void insert_node(NODE *n, SL& sl, NODE** pre_lv) noexcept
    {
        assert(nullptr != n && nullptr != pre_lv);

        // random level
        if (n->get_level() < 0)
            n->set_level(random_level());

        // adjust low-half level
        const int sl_level = sl.get_level(), n_level = n->get_level();
        assert(sl_level >= 0 && n_level >= 0);
        for (int i = 0; i <= sl_level && i <= n_level; ++i)
        {
            if (nullptr == pre_lv[i])
            {
                n->set_next(i, sl.get_head(i));
                sl.set_head(i, n);
            }
            else
            {
                n->set_next(i, pre_lv[i]->get_next(i));
                pre_lv[i]->set_next(i, n);
            }
        }

        // adjust high-half level
        if (n_level > sl_level)
        {
            sl.set_level(n_level);
            for (int i = sl_level + 1; i <= n_level; ++i)
            {
                sl.set_head(i, n);
                n->set_next(i, nullptr);
            }
        }
    }

    /**
     * 移除节点
     *
     * @param n         要插入的节点
     * @param sl        跳表本身
     * @param pre_lv    前向节点数组，长度为 (level+1)
     */
    static void remove_node(NODE *n, SL& sl, NODE **pre_lv) noexcept
    {
        assert(nullptr != n && nullptr != pre_lv);
        const int sl_level = sl.get_level(), n_level = n->get_level();
        assert(sl_level >= 0 && n_level >= 0);
        for (int i = 0; i <= sl_level && i <= n_level; ++i)
        {
            if (nullptr == pre_lv[i])
                sl.set_head(i, n->get_next(i));
            else
                pre_lv[i]->set_next(i, n->get_next(i));
        }
    }

private:
    SkipList() = delete;
};

}

#endif
