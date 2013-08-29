/**
 * @file -
 * @author jingqi
 * @date 2013-08-29
 * @last-edit 2013-08-29 14:39:44 jingqi
 * @brief
 */

#ifndef ___HEADFILE_CD9958A8_95CB_49A7_A294_60B2304EDB7E_
#define ___HEADFILE_CD9958A8_95CB_49A7_A294_60B2304EDB7E_

#include <stdlib.h>

/**
 * skip-list 的公共算法
 *
 * @param K 键值类型，要求能用 "<" 操作符比较大小
 * @param NODE 节点类型，要求实现以下方法
 *      getKey() 获取键值
 *      getLevel() 获取 0-based 层数
 *      setLevel(int) 设置层数，并自动分配 level+1 长度的 next 数组
 *      getNext(int) 获取指定层数的指针
 *      setNext(int,NODE*) 设置指定层数的指针
 * @param SL 跳表数据结构本身，要求实现以下方法
 *      getLevel() 获取跳表 0-based 层数
 *      setLevel(int) 设置层数，并自动分配 level+1 长度的 head 数组
 *      getHead(int) 获取跳表头
 *      setHead(int,NODE*) 设置跳表头
 */
template <typename K, typename NODE, typename SL>
class SkipList
{
    SkipList();

public:
    enum
    {
        // 最大level数, 0-based
        MAX_LEVEL = 16,
        // 无效level
        INVALID_LEVEL = -1
    };

    /**
     * 随机化 level
     *
     * @return 0-based
     */
    static int randomLevel()
    {
        int k = 0;
        while (k < MAX_LEVEL && 0 != (rand() & 0x01))
            ++k;
        return k;
    }

    /**
     * 查找节点，附带查找各层前向节点
     *
     * @param k         要查找的键值
     * @param sl        跳表本身
     * @param pre_lv    存放返回值，可以是 NULL. 存放前向节点的返回值数组，长度为 (level+1).
     */
    static NODE* searchNode(const K& key, const SL& sl, NODE **pre_lv)
    {
        NODE *ret = NULL;
        register NODE *pre = NULL;
        register int lv = sl.getLevel();
        assert(lv >= 0);
        do
        {
            while (true)
            {
                NODE *n = (NULL == pre ? sl.getHead(lv) : pre->getNext(lv));
                if (NULL == n)
                {
                    if (NULL != pre_lv)
                        pre_lv[lv] = pre;
                    break;
                }

                if (key < n->getKey())
                {
                    if (NULL != pre_lv)
                        pre_lv[lv] = pre;
                    break;
                }
                else if (n->getKey() < key)
                {
                    pre = n;
                }
                else
                {
                    if (NULL == pre_lv)
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
    static void insertNode(NODE *n, SL& sl, NODE** pre_lv)
    {
        assert(NULL != n && NULL != pre_lv);

        // random level
        if (n->getLevel() < 0)
            n->setLevel(randomLevel());

        // adjust low-half level
        const int sl_level = sl.getLevel(), n_level = n->getLevel();
        assert(sl_level >= 0 && n_level >= 0);
        for (register int i = 0; i <= sl_level && i <= n_level; ++i)
        {
            if (NULL == pre_lv[i])
            {
                n->setNext(i, sl.getHead(i));
                sl.setHead(i, n);
            }
            else
            {
                n->setNext(i, pre_lv[i]->getNext(i));
                pre_lv[i]->setNext(i, n);
            }
        }

        // adjust high-half level
        if (n_level > sl_level)
        {
            sl.setLevel(n_level);
            for (register int i = sl_level + 1; i <= n_level; ++i)
            {
                sl.setHead(i, n);
                n->setNext(i, NULL);
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
    static void removeNode(NODE *n, SL& sl, NODE **pre_lv)
    {
        assert(NULL != n && NULL != pre_lv);
        const int sl_level = sl.getLevel(), n_level = n->getLevel();
        assert(sl_level >= 0 && n_level >= 0);
        for (register int i = 0; i <= sl_level && i <= n_level; ++i)
        {
            if (NULL == pre_lv[i])
                sl.setHead(i, n->getNext(i));
            else
                pre_lv[i]->setNext(i, n->getNext(i));
        }
    }
};

#endif
