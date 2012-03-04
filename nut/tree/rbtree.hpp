/**
 * @file -
 * @author jingqi
 * @date 2012-02-13
 * @last-edit 2012-03-02 21:19:53 jingqi
 */

#ifndef ___HEADFILE_16D8579F_C3D1_4CE7_ACA2_3C4B4B82E45D_
#define ___HEADFILE_16D8579F_C3D1_4CE7_ACA2_3C4B4B82E45D_

#include <assert.h>

namespace nut
{

/**
 * Red-Black-Tree
 */
template <typename T>
class RBTree
{
protected:
    struct RBTreeNode
    {
        T data;
        RBTreeNode *parent;
        RBTreeNode *left;
        RBTreeNode *right;
        bool red;

        RBTreeNode(const T& v)
            : data(v), parent(NULL), left(NULL), right(NULL), red(true)
        {}
    };

    RBTreeNode *m_root;
    size_t m_size;

public:
    RBTree()
        : m_root(NULL), m_size(0)
    {}

    size_t size() const { return m_size; }

    /**
     * insert a tree node into RBTree and fixup balance
     */
    void rb_insert(RBTreeNode *x)
    {
        assert(NULL != x);
        RBTreeNode *parent = NULL, *current = m_root;
        while (NULL != current)
        {
            parent = current;
            if (x->data < current->data)
                current = current->left;
            else
                current = current->right;
        }

        x->parent = parent;
        if (NULL == parent)
            m_root = x;
        else if (x->data < parent->data)
            parent->left = x;
        else
            parent->right = x;

        x->left = NULL;
        x->right = NULL;
        x->red = true;
        _rb_insert_fixup(x);
        ++m_size;
    }

    /**
     * remove a tree node from RBTree and fixup balance
     */
    void rb_delete(RBTreeNode *x)
    {
        assert(NULL != x);
        RBTreeNode *escaper = NULL;
        if (NULL == x->left || NULL == x->right)
            escaper = x;
        else
            escaper = _tree_successor(x);

        RBTreeNode *sublink = NULL;
        if (NULL != escaper->left)
            sublink = escaper->left;
        else
            sublink = escaper->right;

        RBTreeNode *sublink_parent = escaper->parent;
        if (NULL != sublink)
            sublink->parent = sublink_parent;

        if (NULL == sublink_parent)
            m_root = sublink;
        else if (escaper == sublink_parent->left)
            sublink_parent->left = sublink;
        else
            sublink_parent->right = sublink;

        bool red_escaper = escaper->red;
        if (escaper != x)
        {
            // replace x with escaper
            escaper->left = x->left;
            escaper->right = x->right;
            escaper->parent = x->parent;
            escaper->red = x->red;
            if (NULL == x->parent)
                m_root = escaper;
            else if (x == x->parent->left)
                x->parent->left = escaper;
            else
                x->parent->right = escaper;
        }

        if (!red_escaper)
            _rb_delete_fixup(sublink, sublink_parent);
        --m_size;
    }

private:
    /**
     * get minimum node of binary search tree x
     */
    static inline RBTreeNode* _tree_minimum(RBTreeNode *x)
    {
        assert(NULL != x);
        while (NULL != x->left)
            x = x->left;
        return x;
    }

    /**
     * get maximum node of binary search tree x
     */
    static inline RBTreeNode* _tree_maximum(RBTreeNode *x)
    {
        assert(NULL != x);
        while (NULL != x->right)
            x = x->right;
        return x;
    }

    /**
     * get next node in value sequence
     */
    static RBTreeNode* _tree_successor(RBTreeNode *x)
    {
        assert(NULL != x);
        if (NULL != x->right)
            return _tree_minimum(x->right);
        RBTreeNode *parent = x->parent;
        while (NULL != parent && x == parent->right)
        {
            x = parent;
            parent = x->parent;
        }
        return parent;
    }

    void _left_rotate(RBTreeNode *x)
    {
        //
        //       |                           |
        //       X                           Y
        //      / \     left-rotate(X)      / \
        //         Y         ->            X
        //        / \                     / \
        //
        assert(NULL != x);
        RBTreeNode *y = x->right;
        x->right = y->left;
        if (NULL != y->left)
            y->left->parent = x;
        y->parent = x->parent;
        if (NULL == x->parent)
            m_root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void _right_rotate(RBTreeNode *x)
    {
        //
        //        |                           |
        //        X                           Y
        //       / \     right-rotate(X)     / \
        //      Y             ->                X
        //     / \                             / \
        //
        assert(NULL != x);
        RBTreeNode *y = x.left;
        x->left = y->right;
        if (NULL != y->right)
            y->right->parent = x;
        y->parent = x->parent;
        if (NULL == x->parent)
            m_root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->right = x;
        x->parent = y;
    }

    void _rb_insert_fixup(RBTreeNode *x)
    {
        assert(NULL != x && x->red);
        while (NULL != x->parent && x->parent->red)
        {
            RBTreeNode *parent = x->parent;
            assert(NULL != parent->parent); // because the root is always black
            if (parent == parent->parent->left)
            {
                RBTreeNode *uncle = parent->parent->right;
                if (NULL != uncle && uncle->red)
                {
                    // case 1:
                    //
                    //       |                   |
                    //       B                  [R]
                    //      / \      color      / \
                    //     R   R      ->       B   B
                    //      \                   \
                    //      [R]                  R
                    //
                    parent->red = false;
                    uncle->red = false;
                    parent->parent->red = true;
                    x = parent->parent;
                }
                else
                {
                    if (x == parent->right)
                    {
                        // case 2:
                        //
                        //      |                  |
                        //      B                  B
                        //     / \     rotate     / \
                        //    R   B      ->      R   B
                        //     \                /
                        //     [R]            [R]
                        //
                        x = parent;
                        _left_rotate(x);
                    }

                    // case 3:
                    //
                    //         |                         |
                    //         B                         B
                    //        / \     color & rotate    / \
                    //       R   B          ->        [R]  R
                    //      /                               \
                    //     [R]                               B
                    //
                    parent->red = false;
                    parent->parent->red = true;
                    _right_rotate(parent->parent);
                }
            }
            else
            {
                RBTreeNode *uncle = parent->parent->left;
                if (NULL != uncle && uncle->red)
                {
                    // case 1:
                    //
                    //       |                   |
                    //       B                  [R]
                    //      / \      color      / \
                    //     R   R      ->       B   B
                    //        /                   /
                    //      [R]                  R
                    //
                    parent->red = false;
                    uncle->red = false;
                    parent->parent->red = true;
                    x = parent->parent;
                }
                else
                {
                    if (x == parent->left)
                    {
                        // case 2:
                        //
                        //      |                  |
                        //      B                  B
                        //     / \     rotate     / \
                        //    B   R      ->      B   R
                        //       /                    \
                        //     [R]                    [R]
                        //
                        x = parent;
                        _right_rotate(x);
                    }

                    // case 3:
                    //
                    //         |                         |
                    //         B                         B
                    //        / \     color & rotate    / \
                    //       B   R          ->         R  [R]
                    //            \                   /
                    //            [R]                B
                    //
                    parent->red = false;
                    parent->parent->red = true;
                    _left_rotate(parent->parent);
                }
            }
        }
        m_root->red = false; // root is always black
    }

    void _rb_delete_fixup(RBTreeNode *sublink, RBTreeNode *sublink_parent)
    {
        while (sublink != m_root && (NULL == sublink || !sublink->red))
        {
            assert(NULL != sublink_parent); // because sublink is not root
            if (sublink == sublink_parent->left)
            {
                RBTreeNode *brother = sublink_parent->right;
                assert(NULL != brother); // because before deleting, there must be a right branch here to ensure 
                // the same black height with left branch

                if (brother->red)
                {
                    // case 1:
                    //
                    //     |                                 |
                    //     B                                 B
                    //    / \        color & rotate         / \
                    //  [B]  R            ->               R   B
                    //      / \                           / \
                    //     B   B                        [B]  B
                    //
                    brother->red = false;
                    sublink_parent->red = true;
                    _left_rotate(sublink_parent);
                    brother = sublink_parent->right;
                }

                if ((NULL == brother->left || !brother->left->red) && (NULL == brother->right || !brother->right->red))
                {
                    // case 2:
                    //
                    //    |                      |
                    //    ?                     [?]
                    //   / \         color      / \
                    // [B]  B         ->       B   R
                    //     / \                    / \
                    //    B   B                  B   B
                    //
                    brother->red = true;
                    sublink = sublink_parent;
                    sublink_parent = sublink->parent;
                }
                else
                {
                    if (NULL == brother->right || !brother->right->red)
                    {
                        // case 3:
                        //
                        //       |                             |
                        //       ?                             ?
                        //      / \       color & rotate      / \
                        //    [B]  B            ->          [B]  B
                        //        / \                             \
                        //       R   B                             R
                        //                                          \
                        //                                           B
                        //
                        brother->left->red = false;
                        brother->red = true;
                        _right_rotate(brother);
                        brother = sublink_parent->right;
                    }

                    // case 4:
                    //
                    //     |                                     |
                    //     ?                                     ?
                    //    / \            color & rotate         / \
                    //  [B]  B                ->               B   B
                    //      / \                               / \
                    //     ?   R                             B   ?
                    //
                    brother->red = sublink_parent->red;
                    sublink_parent->red = false;
                    brother->right->red = false;
                    _left_rotate(sublink_parent);
                    sublink = m_root; // end the loop
                    sublink_parent = NULL;
               }
            }
            else
            {
                RBTreeNode *brother = sublink_parent->left;
                assert(NULL != brother); // because before deleting, there must be a left branch here to ensure 
                // the same black height with right branch

                if (brother->red)
                {
                    // case 1:
                    //
                    //     |                                 |
                    //     B                                 B
                    //    / \        color & rotate         / \
                    //   R  [B]            ->              B   R
                    //  / \                                   / \
                    // B   B                                 B  [B]
                    //
                    brother->red = false;
                    sublink_parent->red = true;
                    _right_rotate(sublink_parent);
                    brother = sublink_parent->left;
                }

                if ((NULL == brother->left || !brother->left->red) && (NULL == brother->right || !brother->right->red))
                {
                    // case 2:
                    //
                    //     |                      |
                    //     ?                     [?]
                    //    / \         color      / \
                    //   B  [B]        ->       R   B
                    //  / \                    / \
                    // B   B                  B   B
                    //
                    brother->red = true;
                    sublink = sublink_parent;
                    sublink_parent = sublink->parent;
                }
                else
                {
                    if (NULL == brother->left || !brother->left->red)
                    {
                        // case 3:
                        //
                        //       |                             |
                        //       ?                             ?
                        //      / \       color & rotate      / \
                        //     B  [B]            ->          B  [B]
                        //    / \                           /
                        //   B   R                         R
                        //                                /
                        //                               B
                        //
                        brother->right->red = false;
                        brother->red = true;
                        _left_rotate(brother);
                        brother = sublink_parent->left;
                    }

                    // case 4:
                    //
                    //     |                                     |
                    //     ?                                     ?
                    //    / \            color & rotate         / \
                    //   B  [B]               ->               B   B
                    //  / \                                       / \
                    // R   ?                                     ?   B
                    //
                    brother->red = sublink_parent->red;
                    sublink_parent->red = false;
                    brother->left->red = false;
                    _right_rotate(sublink_parent);
                    sublink = m_root; // end the loop
                    sublink_parent = NULL;
                }
            }
        }
        sublink->red = false;
    }
};

}

#endif /* head file guarder */

