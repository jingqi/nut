﻿
#ifndef ___HEADFILE_CFEF4EB8_082C_417F_A58C_9D65C9F3DAD1_
#define ___HEADFILE_CFEF4EB8_082C_417F_A58C_9D65C9F3DAD1_

#include <assert.h>

#include "../comparable.h"


namespace nut
{

/**
 * 二叉查找树
 *
 * @param NODE 树节点类型, 要求实现以下方法
 *      const K& get_key() const
 *      NODE* get_parent() const
 *      NODE* get_left_child() const
 *      NODE* get_right_child() const
 *      void set_parent(NODE*)
 *      void set_left_child(NODE*)
 *      void set_right_child(NODE*)
 */
template <typename K, typename NODE>
class BSTree
{
public:
    /**
     * 查找数据所在的节点
     *
     * @return 没有找到则返回 nullptr
     */
    static NODE* search(NODE *sub_root, const K& key) noexcept
    {
        while (nullptr != sub_root)
        {
            const int rs = compare(key, sub_root->get_key());
            if (rs < 0)
                sub_root = sub_root->get_left_child();
            else if (rs > 0)
                sub_root = sub_root->get_right_child();
            else
                return sub_root;
        }
        return nullptr;
    }

    /**
     * 插入新节点到二叉查找树
     *
     * @return 新的根
     */
    static NODE* insert(NODE *root, NODE *new_node) noexcept
    {
        assert(nullptr != new_node && nullptr == new_node->get_left_child() &&
               nullptr == new_node->get_right_child());

        NODE *parent = nullptr;
        bool insert_to_left = true;
        for (NODE *x = root; nullptr != x; )
        {
            parent = x;
            if (compare(new_node->get_key(), x->get_key()) < 0)
            {
                x = x->get_left_child();
                insert_to_left = true;
            }
            else
            {
                x = x->get_right_child();
                insert_to_left = false;
            }
        }

        new_node->set_parent(parent);
        if (nullptr == parent)
            root = new_node;
        else if (insert_to_left)
            parent->set_left_child(new_node);
        else
            parent->set_right_child(new_node);
        return root;
    }

    /**
     * 从二叉查找树中删除已有节点
     *
     * @return 新的根
     */
    static NODE* remove(NODE *root, NODE *to_be_del) noexcept
    {
        assert(nullptr != root && nullptr != to_be_del);

        NODE *scapegoat = nullptr;
        if (nullptr == to_be_del->get_left_child() || nullptr == to_be_del->get_right_child())
            scapegoat = to_be_del;
        else
            scapegoat = successor(to_be_del);
        assert(nullptr != scapegoat);

        NODE *sublink = nullptr;
        if (nullptr != scapegoat->get_left_child())
            sublink = scapegoat->get_left_child();
        else
            sublink = scapegoat->get_right_child();

        NODE *const sublink_parent = scapegoat->get_parent();
        if (nullptr != sublink)
            sublink->set_parent(sublink_parent);

        assert((nullptr == sublink_parent) == (root == scapegoat));
        if (nullptr == sublink_parent)
            root = sublink;
        else if (scapegoat == sublink_parent->get_left_child())
            sublink_parent->set_left_child(sublink);
        else
            sublink_parent->set_right_child(sublink);

        if (scapegoat != to_be_del)
        {
            // Replace 'to_be_del' with 'scapegoat'
            NODE *const x_parent = to_be_del->get_parent(),
                *const x_left_child = to_be_del->get_left_child(),
                *const x_right_child = to_be_del->get_right_child();

            scapegoat->set_parent(x_parent);
            scapegoat->set_left_child(x_left_child);
            scapegoat->set_right_child(x_right_child);

            assert((nullptr == x_parent) == (root == to_be_del));
            if (nullptr == x_parent)
                root = scapegoat;
            else if (to_be_del == x_parent->get_left_child())
                x_parent->set_left_child(scapegoat);
            else
                x_parent->set_right_child(scapegoat);

            if (nullptr != x_left_child)
                x_left_child->set_parent(scapegoat);
            if (nullptr != x_right_child)
                x_right_child->set_parent(scapegoat);
        }

        to_be_del->set_parent(nullptr);
        to_be_del->set_left_child(nullptr);
        to_be_del->set_right_child(nullptr);

        return root;
    }

    /**
     * 找到最小数据所在的节点
     */
    static NODE* minimum(NODE *sub_root) noexcept
    {
        assert(nullptr != sub_root);
        while (nullptr != sub_root->get_left_child())
            sub_root = sub_root->get_left_child();
        return sub_root;
    }

    /**
     * 找到最大数据所在的节点
     */
    static NODE* maximum(NODE *sub_root) noexcept
    {
        assert(nullptr != sub_root);
        while (nullptr != sub_root->get_right_child())
            sub_root = sub_root->get_right_child();
        return sub_root;
    }

    /**
     * 按照数据大小顺序找到下一个结点
     */
    static NODE* successor(NODE *x) noexcept
    {
        assert(nullptr != x);
        if (nullptr != x->get_right_child())
            return minimum(x->get_right_child());
        NODE *parent = x->get_parent();
        while (nullptr != parent && x == parent->get_right_child())
        {
            x = parent;
            parent = x->get_parent();
        }
        return parent;
    }

    /**
     * 按照数据大小顺序找到上一个结点
     */
    static NODE* predecessor(NODE *x) noexcept
    {
        assert(nullptr != x);
        if (nullptr != x->get_left_child())
            return maximum(x->get_left_child());
        NODE *parent = x->get_parent();
        while (nullptr != parent && x == parent->get_left_child())
        {
            x = parent;
            parent = x->get_parent();
        }
        return parent;
    }

private:
    BSTree() = delete;
};

}

#endif /* head file guarder */
