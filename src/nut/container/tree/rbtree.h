/**
 * 红黑树定义：
 * 1. 节点是红色或黑色。
 * 2. 根是黑色。
 * 3. 所有叶子都是黑色(叶子是 nil 节点)。
 * 4. 每个红色节点必须有两个黑色的子节点。(从每个叶子到根的所有路径上不能有两个连续的红色节点。)
 * 5. 从任一节点到其每个叶子的所有简单路径都包含相同数目的黑色节点。
 */

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
        assert(nullptr != new_node && nullptr == new_node->get_left_child() &&
               nullptr == new_node->get_right_child());

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
        assert(nullptr != root && nullptr != to_be_del);

        NODE *scapegoat = nullptr;
        if (nullptr == to_be_del->get_left_child() || nullptr == to_be_del->get_right_child())
            scapegoat = to_be_del;
        else
            scapegoat = BSTree<K,NODE>::successor(to_be_del);
        assert(nullptr != scapegoat);

        NODE *sublink = nullptr;
        if (nullptr != scapegoat->get_left_child())
            sublink = scapegoat->get_left_child();
        else
            sublink = scapegoat->get_right_child();

        NODE *sublink_parent = scapegoat->get_parent();
        if (nullptr != sublink)
            sublink->set_parent(sublink_parent);

        assert((nullptr == sublink_parent) == (root == scapegoat));
        if (nullptr == sublink_parent)
            root = sublink;
        else if (scapegoat == sublink_parent->get_left_child())
            sublink_parent->set_left_child(sublink);
        else
            sublink_parent->set_right_child(sublink);

        const bool red_scapegoat = scapegoat->is_red();
        if (scapegoat != to_be_del)
        {
            // Replace 'to_be_del' with 'scapegoat'
            NODE *const x_parent = to_be_del->get_parent(),
                *const x_left_child = to_be_del->get_left_child(),
                *const x_right_child = to_be_del->get_right_child();

            scapegoat->set_parent(x_parent);
            scapegoat->set_left_child(x_left_child);
            scapegoat->set_right_child(x_right_child);
            scapegoat->set_red(to_be_del->is_red());

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

            // NOTE Don't forget to update 'sublink_parent'
            if (sublink_parent == to_be_del)
                sublink_parent = scapegoat;
        }

        if (!red_scapegoat)
            root = _remove_fixup(root, sublink, sublink_parent);

        to_be_del->set_parent(nullptr);
        to_be_del->set_left_child(nullptr);
        to_be_del->set_right_child(nullptr);

        return root;
    }

private:
    RBTree() = delete;

    /**
     * 左旋转
     *
     *       |                           |
     *       X                           Y
     *      / \     left-rotate(X)      / \
     *         Y         ->            X
     *        / \                     / \
     *       z                           z
     */
    static NODE* _left_rotate(NODE *root, NODE *x) noexcept
    {
        assert(nullptr != root && nullptr != x);

        NODE *const y = x->get_right_child();
        assert(nullptr != y);

        NODE *const z = y->get_left_child();
        x->set_right_child(z);
        if (nullptr != z)
            z->set_parent(x);

        NODE *const parent = x->get_parent();
        assert((nullptr == parent) == (root == x));
        y->set_parent(parent);
        if (nullptr == parent)
            root = y;
        else if (x == parent->get_left_child())
            parent->set_left_child(y);
        else
            parent->set_right_child(y);

        y->set_left_child(x);
        x->set_parent(y);

        return root;
    }

    /**
     * 右旋转
     *
     *        |                           |
     *        X                           Y
     *       / \     right-rotate(X)     / \
     *      Y             ->                X
     *     / \                             / \
     *        z                           z
     */
    static NODE* _right_rotate(NODE *root, NODE *x) noexcept
    {
        assert(nullptr != root && nullptr != x);

        NODE *const y = x->get_left_child();
        assert(nullptr != y);

        NODE *const z = y->get_right_child();
        x->set_left_child(z);
        if (nullptr != z)
            z->set_parent(x);

        NODE *const parent = x->get_parent();
        assert((nullptr == parent) == (root == x));
        y->set_parent(parent);
        if (nullptr == parent)
            root = y;
        else if (x == parent->get_left_child())
            parent->set_left_child(y);
        else
            parent->set_right_child(y);

        y->set_right_child(x);
        x->set_parent(y);

        return root;
    }

    static NODE* _insert_fixup(NODE *root, NODE *x) noexcept
    {
        assert(nullptr != root && nullptr != x && x->is_red() &&
               nullptr == x->get_left_child() && nullptr == x->get_right_child());

        while (nullptr != x->get_parent() && x->get_parent()->is_red())
        {
            NODE *parent = x->get_parent();
            NODE *const grandparent = parent->get_parent();
            assert(nullptr != grandparent); // Because the root is always black
            if (parent == grandparent->get_left_child())
            {
                NODE *uncle = grandparent->get_right_child();
                if (nullptr != uncle && uncle->is_red())
                {
                    /* case 1:
                     *
                     *       |                   |
                     *       B                  [R]
                     *      / \      color      / \
                     *     R   R      ->       B   B
                     *     |                   |
                     *    [R]                  R
                     */
                    parent->set_red(false);
                    uncle->set_red(false);
                    grandparent->set_red(true);
                    x = grandparent;
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
                        root = _left_rotate(root, parent);
                        x = parent;
                        parent = x->get_parent(); // Current 'parent' is original 'x'
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
                    grandparent->set_red(true);
                    root = _right_rotate(root, grandparent);
                }
            }
            else
            {
                NODE *uncle = grandparent->get_left_child();
                if (nullptr != uncle && uncle->is_red())
                {
                    /* case 1:
                     *
                     *       |                   |
                     *       B                  [R]
                     *      / \      color      / \
                     *     R   R      ->       B   B
                     *         |                   |
                     *        [R]                  R
                     */
                    parent->set_red(false);
                    uncle->set_red(false);
                    grandparent->set_red(true);
                    x = grandparent;
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
                        root = _right_rotate(root, parent);
                        x = parent;
                        parent = x->get_parent(); // Current 'parent' is original 'x'
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
                    grandparent->set_red(true);
                    root = _left_rotate(root, parent->get_parent());
                }
            }
        }
        root->set_red(false); // Root is always black
        return root;
    }

    static NODE* _remove_fixup(NODE *root, NODE *sublink, NODE *sublink_parent) noexcept
    {
        while (sublink != root && (nullptr == sublink || !sublink->is_red()))
        {
            assert(nullptr != sublink_parent); // Because sublink is not root
            if (sublink == sublink_parent->get_left_child())
            {
                NODE *brother = sublink_parent->get_right_child();
                assert(nullptr != brother); // Because before deleting, there must be a right branch here to ensure
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
                    sublink = root; // End the loop
                    sublink_parent = nullptr;
               }
            }
            else
            {
                NODE *brother = sublink_parent->get_left_child();
                assert(nullptr != brother); // Because before deleting, there must be a left branch here to ensure
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
                    sublink = root; // End the loop
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
