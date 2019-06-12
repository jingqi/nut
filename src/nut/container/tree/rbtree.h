
#ifndef ___HEADFILE_16D8579F_C3D1_4CE7_ACA2_3C4B4B82E45D_
#define ___HEADFILE_16D8579F_C3D1_4CE7_ACA2_3C4B4B82E45D_

#include <assert.h>

#include "../comparable.h"
#include "bstree.h"


namespace nut
{

/**
 * 红黑树(Red-Black-Tree)
 *
 * @param NODE 树节点类型, 要求实现以下方法
 *      bool is_red() const            获取节点的颜色, true 为红色, false 为黑色
 *      const K& get_key() const
 *      NODE* get_parent() const
 *      NODE* get_left_child() const
 *      NODE* get_right_child() const
 *      void set_red(bool)             设置节点的颜色
 *      void set_parent(NODE*)
 *      void set_left_child(NODE*)
 *      void set_right_child(NODE*)
 */
template <typename K, typename NODE>
class RBTree
{
public:
    /**
     * 插入新节点到红黑树
     *
     * @return 新的根
     */
    static NODE* insert(NODE *root, NODE *new_node) noexcept
    {
        assert(nullptr != new_node);
        NODE *parent = nullptr;
        bool insert_to_left = true;
        for (NODE *current = root; nullptr != current; )
        {
            parent = current;
            if (compare(new_node->get_key(), current->get_key()) < 0)
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
        if (nullptr == parent)
            root = new_node;
        else if (insert_to_left)
            parent->set_left_child(new_node);
        else
            parent->set_right_child(new_node);

        new_node->set_red(true);
        root = _insert_fixup(root, new_node);

        return root;
    }

    /**
     * 从红黑树中删除已有节点
     *
     * @return 新的根
     */
    static NODE* remove(NODE *root, NODE *to_be_del) noexcept
    {
        assert(nullptr != to_be_del);
        NODE *escaper = nullptr;
        if (nullptr == to_be_del->get_left_child() || nullptr == to_be_del->get_right_child())
            escaper = to_be_del;
        else
            escaper = BSTree<K,NODE>::successor(to_be_del);

        NODE *sublink = nullptr;
        if (nullptr != escaper->get_left_child())
            sublink = escaper->get_left_child();
        else
            sublink = escaper->get_right_child();

        NODE *sublink_parent = escaper->get_parent();
        if (nullptr != sublink)
            sublink->set_parent(sublink_parent);

        if (nullptr == sublink_parent)
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
            if (nullptr == to_be_del->get_parent())
                root = escaper;
            else if (to_be_del == to_be_del->get_parent()->get_left_child())
                to_be_del->get_parent()->set_left_child(escaper);
            else
                to_be_del->get_parent()->set_right_child(escaper);
        }

        if (!red_escaper)
            root = _remove_fixup(root, sublink, sublink_parent);

        return root;
    }

private:
    RBTree() = delete;

    /**
     * 左旋转
     */
    static NODE* _left_rotate(NODE *root, NODE *x) noexcept
    {
        /*
         *
         *       |                           |
         *       X                           Y
         *      / \     left-rotate(X)      / \
         *         Y         ->            X
         *        / \                     / \
         */
        assert(nullptr != x);
        NODE *y = x->get_right_child();
        x->set_right_child(y->get_left_child());
        if (nullptr != y->get_left_child())
            y->get_left_child()->set_parent(x);
        y->set_parent(x->get_parent());
        if (nullptr == x->get_parent())
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
    static NODE* _right_rotate(NODE *root, NODE *x) noexcept
    {
        /*
         *        |                           |
         *        X                           Y
         *       / \     right-rotate(X)     / \
         *      Y             ->                X
         *     / \                             / \
         */
        assert(nullptr != x);
        NODE *y = x->get_left_child();
        x->set_left_child(y->get_right_child());
        if (nullptr != y->get_right_child())
            y->get_right_child()->set_parent(x);
        y->set_parent(x->get_parent());
        if (nullptr == x->get_parent())
            root = y;
        else if (x == x->get_parent()->get_left_child())
            x->get_parent()->set_left_child(y);
        else
            x->get_parent()->set_right_child(y);
        y->set_right_child(x);
        x->set_parent(y);

        return root;
    }

    static NODE* _insert_fixup(NODE *root, NODE *x) noexcept
    {
        assert(nullptr != x && x->is_red());
        while (nullptr != x->get_parent() && x->get_parent()->is_red())
        {
            NODE *parent = x->get_parent();
            assert(nullptr != parent->get_parent()); // because the root is always black
            if (parent == parent->get_parent()->get_left_child())
            {
                NODE *uncle = parent->get_parent()->get_right_child();
                if (nullptr != uncle && uncle->is_red())
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
                if (nullptr != uncle && uncle->is_red())
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

    static NODE* _remove_fixup(NODE *root, NODE *sublink, NODE *sublink_parent) noexcept
    {
        while (sublink != root && (nullptr == sublink || !sublink->is_red()))
        {
            assert(nullptr != sublink_parent); // because sublink is not root
            if (sublink == sublink_parent->get_left_child())
            {
                NODE *brother = sublink_parent->get_right_child();
                assert(nullptr != brother); // because before deleting, there must be a right branch here to ensure
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

                if ((nullptr == brother->get_left_child() || !brother->get_left_child()->is_red()) &&
                    (nullptr == brother->get_right_child() || !brother->get_right_child()->is_red()))
                {
                    /* case 2:
                     *
                     *     |                      |
                     *     ?                     [?]
                     *    / \         color      / \
                     *  [B]  B         ->       B   R
                     *      / \                    / \
                     *     B   B                  B   B
                     */
                    brother->set_red(true);
                    sublink = sublink_parent;
                    sublink_parent = sublink->get_parent();
                }
                else
                {
                    if (nullptr == brother->get_right_child() || !brother->get_right_child()->is_red())
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
                    sublink_parent = nullptr;
               }
            }
            else
            {
                NODE *brother = sublink_parent->get_left_child();
                assert(nullptr != brother); // because before deleting, there must be a left branch here to ensure
                // the same black height with right branch

                if (brother->is_red())
                {
                    /* case 1:
                     *
                     *       |                                 |
                     *       B                                 B
                     *      / \        color & rotate         / \
                     *     R  [B]            ->              B   R
                     *    / \                                   / \
                     *   B   B                                 B  [B]
                     */
                    brother->set_red(false);
                    sublink_parent->set_red(true);
                    root = _right_rotate(root, sublink_parent);
                    brother = sublink_parent->get_left_child();
                }

                if ((nullptr == brother->get_left_child() || !brother->get_left_child()->is_red()) &&
                    (nullptr == brother->get_right_child() || !brother->get_right_child()->is_red()))
                {
                    /* case 2:
                     *
                     *        |                      |
                     *        ?                     [?]
                     *       / \         color      / \
                     *      B  [B]        ->       R   B
                     *     / \                    / \
                     *    B   B                  B   B
                     */
                    brother->set_red(true);
                    sublink = sublink_parent;
                    sublink_parent = sublink->get_parent();
                }
                else
                {
                    if (nullptr == brother->get_left_child() || !brother->get_left_child()->is_red())
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
                     *       |                                     |
                     *       ?                                     ?
                     *      / \            color & rotate         / \
                     *     B  [B]               ->               B   B
                     *    / \                                       / \
                     *   R   ?                                     ?   B
                     */
                    brother->set_red(sublink_parent->is_red());
                    sublink_parent->set_red(false);
                    brother->get_left_child()->set_red(false);
                    root = _right_rotate(root, sublink_parent);
                    sublink = root; // end the loop
                    sublink_parent = nullptr;
                }
            }
        }
        if (nullptr != sublink)
            sublink->set_red(false);
        return root;
    }
};

}

#endif /* head file guarder */
