/**
 * @file -
 * @author jingqi
 * @date 2012-03-02
 * @last-edit 2012-03-02 21:21:56 jingqi
 */

#ifndef ___HEADFILE_CFEF4EB8_082C_417F_A58C_9D65C9F3DAD1_
#define ___HEADFILE_CFEF4EB8_082C_417F_A58C_9D65C9F3DAD1_

#include "btree.hpp"

namespace nut
{

/**
 * 二叉查找树
 */
template <typename K, typename NODE>
class BSTree
{
public:

    /**
     * 查找数据所在的节点
     *
     * @return
     *      没有找到则返回 NULL
     */
    static NODE* search(NODE *sub_root, const K& key)
    {
        while (NULL != sub_root)
        {
            if (key < sub_root->key())
                sub_root = sub_root->left();
            else if (sub_root->key() < key)
                sub_root = sub_root->right();
            else
                return sub_root;
        }
        return NULL;
    }

    static NODE* insert(NODE *root, NODE *new_node)
    {
        assert(NULL != new_node);
        NODE *parent = NULL;
        for (NODE *x = root; NULL != x; )
        {
            parent = x;
            if (new_node->key() < x->key())
                x = x->left();
            else
                x = x->right();
        }

        new_node->parent(parent);
        if (NULL == parent)
            return new_node;
        else if (new_node->key() < parent->key())
            parent->left(new_node);
        else
            parent->right(new_node);
        return root;
    }

    static NODE* remove(NODE *root, const K& key)
    {
        // TODO
    }

    /**
     * 找到最小数据所在的节点
     */
    static NODE* minimum(NODE *sub_root)
    {
        assert(NULL != sub_root);
        while (NULL != sub_root->left())
            sub_root = sub_root->left();
        return sub_root;
    }
    
    /**
     * 找到最大数据所在的节点
     */
    static NODE* maximum(NODE *sub_root)
    {
        assert(NULL != sub_root);
        while (NULL != sub_root->right())
            sub_root = sub_root->right();
        return sub_root;
    }

    /**
     * 按照数据大小顺序找到下一个结点
     */
    static NODE* successor(NODE *x)
    {
        assert(NULL != x);
        if (NULL != x->right())
            return minimum(x->right());
        NODE *parent = x->parent();
        while (NULL != parent && x == parent->right())
        {
            x = parent;
            parent = x->parent();
        }
        return parent;
    }
    
    /**
     * 按照数据大小顺序找到上一个结点
     */
    static NODE* predecessor(NODE *x)
    {
        assert(NULL != x);
        if (NULL != x->left())
            return maximum(x->left());
        NODE *parent = x->parent();
        while (NULL != parent && x == parent->left())
        {
            x = parent;
            parent = x->parent();
        }
        return parent;
    }
};

}

#endif /* head file guarder */

