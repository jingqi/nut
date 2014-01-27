/**
 * @file -
 * @author jingqi
 * @date 2012-02-13
 * @last-edit 2013-12-11 17:45:09 jingqi
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
        bool insertToLeft = true;
        for (NODE *current = root; NULL != current; )
        {
            parent = current;
            if (new_node->getKey() < current->getKey())
            {
                current = current->getLeftChild();
                insertToLeft = true;
            }
            else
            {
                current = current->getRightChild();
                insertToLeft = false;
            }
        }

        new_node->setParent(parent);
        if (NULL == parent)
            root = new_node;
        else if (insertToLeft)
            parent->setLeftChild(new_node);
        else
            parent->setRightChild(new_node);

        new_node->setRed(true);
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
        if (NULL == to_be_del->getLeftChild() || NULL == to_be_del->getRightChild())
            escaper = to_be_del;
        else
            escaper = BSTree<K,NODE>::successor(to_be_del);

        NODE *sublink = NULL;
        if (NULL != escaper->getLeftChild())
            sublink = escaper->getLeftChild();
        else
            sublink = escaper->getRightChild();

        NODE *sublink_parent = escaper->getParent();
        if (NULL != sublink)
            sublink->setParent(sublink_parent);

        if (NULL == sublink_parent)
            root = sublink;
        else if (escaper == sublink_parent->getLeftChild())
            sublink_parent->setLeftChild(sublink);
        else
            sublink_parent->setRightChild(sublink);

        const bool red_escaper = escaper->isRed();
        if (escaper != to_be_del)
        {
            // replace x with escaper
            escaper->setLeftChild(to_be_del->getLeftChild());
            escaper->setRightChild(to_be_del->getRightChild());
            escaper->setParent(to_be_del->getParent());
            escaper->setRed(to_be_del->isRed);
            if (NULL == to_be_del->getParent())
                root = escaper;
            else if (to_be_del == to_be_del->getParent()->getLeftChild())
                to_be_del->getParent()->setLeftChild(escaper);
            else
                to_be_del->getParent()->setRightChild(escaper);
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
        NODE *y = x->getRightChild();
        x->setRightChild(y->getLeftChild());
        if (NULL != y->getLeftChild())
            y->getLeftChild()->setParent(x);
        y->setParent(x->getParent());
        if (NULL == x->getParent())
            root = y;
        else if (x == x->getParent()->getLeftChild())
            x->getParent()->setLeftChild(y);
        else
            x->getParent()->setRightChild(y);
        y->setLeftChild(x);
        x->setParent(y);

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
        NODE *y = x->getLeftChild();
        x->setLeftChild(y->getRightChild());
        if (NULL != y->getRightChild())
            y->getRightChild()->setParent(x);
        y->setParent(x->getParent());
        if (NULL == x->getParent())
            root = y;
        else if (x == x->getParent()->getLeftChild())
            x->getParent()->setLeftChild(y);
        else
            x->getParent()->setRightChild(y);
        y->setRightChild(x);
        x->setParent(y);

        return root;
    }

    static NODE* _rb_insert_fixup(NODE *root, NODE *x)
    {
        assert(NULL != x && x->isRed());
        while (NULL != x->getParent() && x->getParent()->isRed())
        {
            NODE *parent = x->getParent();
            assert(NULL != parent->getParent()); // because the root is always black
            if (parent == parent->getParent()->getLeftChild())
            {
                NODE *uncle = parent->getParent()->getRightChild();
                if (NULL != uncle && uncle->isRed())
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
                    parent->setRed(false);
                    uncle->setRed(false);
                    parent->getParent()->setRed(true);
                    x = parent->getParent();
                }
                else
                {
                    if (x == parent->getRightChild())
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
                    parent->setRed(false);
                    parent->getParent()->setRed(true);
                    root = _right_rotate(root, parent->getParent());
                }
            }
            else
            {
                NODE *uncle = parent->getParent()->getLeftChild();
                if (NULL != uncle && uncle->isRed())
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
                    parent->setRed(false);
                    uncle->setRed(false);
                    parent->getParent()->setRed(true);
                    x = parent->getParent();
                }
                else
                {
                    if (x == parent->getLeftChild())
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
                    parent->setRed(false);
                    parent->getParent()->setRed(true);
                    root = _left_rotate(root, parent->getParent());
                }
            }
        }
        root->setRed(false); // root is always black
        return root;
    }

    static NODE* _rb_delete_fixup(NODE *root, NODE *sublink, NODE *sublink_parent)
    {
        while (sublink != root && (NULL == sublink || !sublink->isRed()))
        {
            assert(NULL != sublink_parent); // because sublink is not root
            if (sublink == sublink_parent->getLeftChild())
            {
                NODE *brother = sublink_parent->getRightChild();
                assert(NULL != brother); // because before deleting, there must be a right branch here to ensure 
                // the same black height with left branch

                if (brother->isRed())
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
                    brother->setRed(false);
                    sublink_parent->setRed(true);
                    root = _left_rotate(root, sublink_parent);
                    brother = sublink_parent->getRightChild();
                }

                if ((NULL == brother->getLeftChild() || !brother->getLeftChild()->isRed()) && (NULL == brother->getRightChild() || !brother->getRightChild()->isRed()))
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
                    brother->setRed(true);
                    sublink = sublink_parent;
                    sublink_parent = sublink->getParent();
                }
                else
                {
                    if (NULL == brother->getRightChild() || !brother->getRightChild()->isRed())
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
                        brother->getLeftChild()->setRed(false);
                        brother->setRed(true);
                        root = _right_rotate(root, brother);
                        brother = sublink_parent->getRightChild();
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
                    brother->setRed(sublink_parent->isRed());
                    sublink_parent->setRed(false);
                    brother->getRightChild()->setRed(false);
                    root = _left_rotate(root, sublink_parent);
                    sublink = root; // end the loop
                    sublink_parent = NULL;
               }
            }
            else
            {
                NODE *brother = sublink_parent->getLeftChild();
                assert(NULL != brother); // because before deleting, there must be a left branch here to ensure 
                // the same black height with right branch

                if (brother->isRed())
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
                    brother->setRed(false);
                    sublink_parent->setRed(true);
                    root = _right_rotate(root, sublink_parent);
                    brother = sublink_parent->getLeftChild();
                }

                if ((NULL == brother->getLeftChild() || !brother->getLeftChild()->isRed()) && (NULL == brother->getRightChild() || !brother->getRightChild()->isRed()))
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
                    brother->setRed(true);
                    sublink = sublink_parent;
                    sublink_parent = sublink->getParent();
                }
                else
                {
                    if (NULL == brother->getLeftChild() || !brother->getLeftChild()->isRed())
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
                        brother->getRightChild()->setRed(false);
                        brother->setRed(true);
                        root = _left_rotate(root, brother);
                        brother = sublink_parent->getLeftChild();
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
                    brother->setRed(sublink_parent->isRed());
                    sublink_parent->setRed(false);
                    brother->getLeftChild()->setRed(false);
                    root = _right_rotate(root, sublink_parent);
                    sublink = root; // end the loop
                    sublink_parent = NULL;
                }
            }
        }
        sublink->setRed(false);
        return root;
    }
};

}

#endif /* head file guarder */

