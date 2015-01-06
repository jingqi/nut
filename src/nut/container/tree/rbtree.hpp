/**
 * @file -
 * @author jingqi
 * @date 2012-02-13
 * @last-edit 2015-01-06 18:59:16 jingqi
 */

#ifndef ___HEADFILE_16D8579F_C3D1_4CE7_ACA2_3C4B4B82E45D_
#define ___HEADFILE_16D8579F_C3D1_4CE7_ACA2_3C4B4B82E45D_

#include <assert.h>

#include "bstree.hpp"

namespace nut
{

/**
 * Red-Black-Tree
 */
template <typename K, typename NODE>
class RBTree
{
public:
    /**
     * 插入新节点到二叉查找树
     *
     * @return 新的根
     */
    static NODE* insert(NODE *root, NODE *new_node)
    {
        assert(NULL != new_node);
        NODE *parent = NULL;
        bool insert_to_left = true;
        for (NODE *current = root; NULL != current; )
        {
            parent = current;
            if (new_node->get_key() < current->get_key())
            {
                current = current->get_left_child();
                insert_to_left = true;
            }
            else
            {
                current = current->get_right_child();
                insert_to_left = false;
            }
        }

        new_node->set_parent(parent);
        if (NULL == parent)
            root = new_node;
        else if (insert_to_left)
            parent->set_left_child(new_node);
        else
            parent->set_right_child(new_node);

        new_node->set_red(true);
        root = _rb_insert_fixup(root, new_node);

        return root;
    }

    /**
     * 从红黑树中删除已有节点
     *
     * @return 新的根
     */
    static NODE* remove(NODE *root, NODE *to_be_del)
    {
        assert(NULL != to_be_del);
        NODE *escaper = NULL;
        if (NULL == to_be_del->get_left_child() || NULL == to_be_del->get_right_child())
            escaper = to_be_del;
        else
            escaper = BSTree<K,NODE>::successor(to_be_del);

        NODE *sublink = NULL;
        if (NULL != escaper->get_left_child())
            sublink = escaper->get_left_child();
        else
            sublink = escaper->get_right_child();

        NODE *sublink_parent = escaper->get_parent();
        if (NULL != sublink)
            sublink->set_parent(sublink_parent);

        if (NULL == sublink_parent)
            root = sublink;
        else if (escaper == sublink_parent->get_left_child())
            sublink_parent->set_left_child(sublink);
        else
            sublink_parent->set_right_child(sublink);

        const bool red_escaper = escaper->is_red();
        if (escaper != to_be_del)
        {
            // replace x with escaper
            escaper->set_left_child(to_be_del->get_left_child());
            escaper->set_right_child(to_be_del->get_right_child());
            escaper->set_parent(to_be_del->get_parent());
            escaper->set_red(to_be_del->is_red());
            if (NULL == to_be_del->get_parent())
                root = escaper;
            else if (to_be_del == to_be_del->get_parent()->get_left_child())
                to_be_del->get_parent()->set_left_child(escaper);
            else
                to_be_del->get_parent()->set_right_child(escaper);
        }

        if (!red_escaper)
            root = _rb_delete_fixup(root, sublink, sublink_parent);

        return root;
    }

private:
    /**
     * 左旋转
     */
    static NODE* _left_rotate(NODE *root, NODE *x)
    {
        /*
         *
         *       |                           |
         *       X                           Y
         *      / \     left-rotate(X)      / \
         *         Y         ->            X
         *        / \                     / \
         */
        assert(NULL != x);
        NODE *y = x->get_right_child();
        x->set_right_child(y->get_left_child());
        if (NULL != y->get_left_child())
            y->get_left_child()->set_parent(x);
        y->set_parent(x->get_parent());
        if (NULL == x->get_parent())
            root = y;
        else if (x == x->get_parent()->get_left_child())
            x->get_parent()->set_left_child(y);
        else
            x->get_parent()->set_right_child(y);
        y->set_left_child(x);
        x->set_parent(y);

        return root;
    }

    /**
     * 右旋转
     */
    static NODE* _right_rotate(NODE *root, NODE *x)
    {
        /*
         *        |                           |
         *        X                           Y
         *       / \     right-rotate(X)     / \
         *      Y             ->                X
         *     / \                             / \
         */
        assert(NULL != x);
        NODE *y = x->get_left_child();
        x->set_left_child(y->get_right_child());
        if (NULL != y->get_right_child())
            y->get_right_child()->set_parent(x);
        y->set_parent(x->get_parent());
        if (NULL == x->get_parent())
            root = y;
        else if (x == x->get_parent()->get_left_child())
            x->get_parent()->set_left_child(y);
        else
            x->get_parent()->set_right_child(y);
        y->set_right_child(x);
        x->set_parent(y);

        return root;
    }

    static NODE* _rb_insert_fixup(NODE *root, NODE *x)
    {
        assert(NULL != x && x->is_red());
        while (NULL != x->get_parent() && x->get_parent()->is_red())
        {
            NODE *parent = x->get_parent();
            assert(NULL != parent->get_parent()); // because the root is always black
            if (parent == parent->get_parent()->get_left_child())
            {
                NODE *uncle = parent->get_parent()->get_right_child();
                if (NULL != uncle && uncle->is_red())
                {
                    /* case 1:
                     *
                     *       |                   |
                     *       B                  [R]
                     *      / \      color      / \
                     *     R   R      ->       B   B
                     *      \                   \
                     *      [R]                  R
                     */
                    parent->set_red(false);
                    uncle->set_red(false);
                    parent->get_parent()->set_red(true);
                    x = parent->get_parent();
                }
                else
                {
                    if (x == parent->get_right_child())
                    {
                        /* case 2:
                         *
                         *      |                  |
                         *      B                  B
                         *     / \     rotate     / \
                         *    R   B      ->      R   B
                         *     \                /
                         *     [R]            [R]
                         */
                        x = parent;
                        root = _left_rotate(root, x);
                    }

                    /* case 3:
                     *
                     *         |                         |
                     *         B                         B
                     *        / \     color & rotate    / \
                     *       R   B          ->        [R]  R
                     *      /                               \
                     *     [R]                               B
                     */
                    parent->set_red(false);
                    parent->get_parent()->set_red(true);
                    root = _right_rotate(root, parent->get_parent());
                }
            }
            else
            {
                NODE *uncle = parent->get_parent()->get_left_child();
                if (NULL != uncle && uncle->is_red())
                {
                    /* case 1:
                     *
                     *       |                   |
                     *       B                  [R]
                     *      / \      color      / \
                     *     R   R      ->       B   B
                     *        /                   /
                     *      [R]                  R
                     */
                    parent->set_red(false);
                    uncle->set_red(false);
                    parent->get_parent()->set_red(true);
                    x = parent->get_parent();
                }
                else
                {
                    if (x == parent->get_left_child())
                    {
                        /* case 2:
                         *
                         *      |                  |
                         *      B                  B
                         *     / \     rotate     / \
                         *    B   R      ->      B   R
                         *       /                    \
                         *     [R]                    [R]
                         */
                        x = parent;
                        root = _right_rotate(root, x);
                    }

                    /* case 3:
                     *
                     *         |                         |
                     *         B                         B
                     *        / \     color & rotate    / \
                     *       B   R          ->         R  [R]
                     *            \                   /
                     *            [R]                B
                     */
                    parent->set_red(false);
                    parent->get_parent()->set_red(true);
                    root = _left_rotate(root, parent->get_parent());
                }
            }
        }
        root->set_red(false); // root is always black
        return root;
    }

    static NODE* _rb_delete_fixup(NODE *root, NODE *sublink, NODE *sublink_parent)
    {
        while (sublink != root && (NULL == sublink || !sublink->is_red()))
        {
            assert(NULL != sublink_parent); // because sublink is not root
            if (sublink == sublink_parent->get_left_child())
            {
                NODE *brother = sublink_parent->get_right_child();
                assert(NULL != brother); // because before deleting, there must be a right branch here to ensure
                // the same black height with left branch

                if (brother->is_red())
                {
                    /* case 1:
                     *
                     *     |                                 |
                     *     B                                 B
                     *    / \        color & rotate         / \
                     *  [B]  R            ->               R   B
                     *      / \                           / \
                     *     B   B                        [B]  B
                     */
                    brother->set_red(false);
                    sublink_parent->set_red(true);
                    root = _left_rotate(root, sublink_parent);
                    brother = sublink_parent->get_right_child();
                }

                if ((NULL == brother->get_left_child() || !brother->get_left_child()->is_red()) && (NULL == brother->get_right_child() || !brother->get_right_child()->is_red()))
                {
                    /* case 2:
                     *
                     *    |                      |
                     *    ?                     [?]
                     *   / \         color      / \
                     * [B]  B         ->       B   R
                     *     / \                    / \
                     *    B   B                  B   B
                     */
                    brother->set_red(true);
                    sublink = sublink_parent;
                    sublink_parent = sublink->get_parent();
                }
                else
                {
                    if (NULL == brother->get_right_child() || !brother->get_right_child()->is_red())
                    {
                        /* case 3:
                         *
                         *       |                             |
                         *       ?                             ?
                         *      / \       color & rotate      / \
                         *    [B]  B            ->          [B]  B
                         *        / \                             \
                         *       R   B                             R
                         *                                          \
                         *                                           B
                         */
                        brother->get_left_child()->set_red(false);
                        brother->set_red(true);
                        root = _right_rotate(root, brother);
                        brother = sublink_parent->get_right_child();
                    }

                    /* case 4:
                     *
                     *     |                                     |
                     *     ?                                     ?
                     *    / \            color & rotate         / \
                     *  [B]  B                ->               B   B
                     *      / \                               / \
                     *     ?   R                             B   ?
                     */
                    brother->set_red(sublink_parent->is_red());
                    sublink_parent->set_red(false);
                    brother->get_right_child()->set_red(false);
                    root = _left_rotate(root, sublink_parent);
                    sublink = root; // end the loop
                    sublink_parent = NULL;
               }
            }
            else
            {
                NODE *brother = sublink_parent->get_left_child();
                assert(NULL != brother); // because before deleting, there must be a left branch here to ensure
                // the same black height with right branch

                if (brother->is_red())
                {
                    /* case 1:
                     *
                     *     |                                 |
                     *     B                                 B
                     *    / \        color & rotate         / \
                     *   R  [B]            ->              B   R
                     *  / \                                   / \
                     * B   B                                 B  [B]
                     */
                    brother->set_red(false);
                    sublink_parent->set_red(true);
                    root = _right_rotate(root, sublink_parent);
                    brother = sublink_parent->get_left_child();
                }

                if ((NULL == brother->get_left_child() || !brother->get_left_child()->is_red()) && (NULL == brother->get_right_child() || !brother->get_right_child()->is_red()))
                {
                    /* case 2:
                     *
                     *     |                      |
                     *     ?                     [?]
                     *    / \         color      / \
                     *   B  [B]        ->       R   B
                     *  / \                    / \
                     * B   B                  B   B
                     */
                    brother->set_red(true);
                    sublink = sublink_parent;
                    sublink_parent = sublink->get_parent();
                }
                else
                {
                    if (NULL == brother->get_left_child() || !brother->get_left_child()->is_red())
                    {
                        /* case 3:
                         *
                         *       |                             |
                         *       ?                             ?
                         *      / \       color & rotate      / \
                         *     B  [B]            ->          B  [B]
                         *    / \                           /
                         *   B   R                         R
                         *                                /
                         *                               B
                         */
                        brother->get_right_child()->set_red(false);
                        brother->set_red(true);
                        root = _left_rotate(root, brother);
                        brother = sublink_parent->get_left_child();
                    }

                    /* case 4:
                     *
                     *     |                                     |
                     *     ?                                     ?
                     *    / \            color & rotate         / \
                     *   B  [B]               ->               B   B
                     *  / \                                       / \
                     * R   ?                                     ?   B
                     */
                    brother->set_red(sublink_parent->is_red());
                    sublink_parent->set_red(false);
                    brother->get_left_child()->set_red(false);
                    root = _right_rotate(root, sublink_parent);
                    sublink = root; // end the loop
                    sublink_parent = NULL;
                }
            }
        }
        sublink->set_red(false);
        return root;
    }
};

}

#endif /* head file guarder */
