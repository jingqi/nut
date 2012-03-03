/**
 * @file -
 * @author jingqi
 * @date 2012-03-02
 * @last-edit 2012-03-02 21:31:10 jingqi
 */

#ifndef ___HEADFILE_BDA2D5F4_E926_4BBB_A415_DE4463C47D1A_
#define ___HEADFILE_BDA2D5F4_E926_4BBB_A415_DE4463C47D1A_

#include <assert.h>

namespace nut
{

/**
 * 二叉树
 */
template <typename NODE>
class BTree
{
public:

    /**
     * 中序遍历迭代器
     *
     *        B
     *       / \
     *      A   C
     */
    class InorderTraversalIterator
    {
        const NODE *m_parent_of_sub_root;
        NODE *m_current;
        bool m_eof;

    public:
        InorderTraversalIterator(NODE *parent_of_sub_root, NODE *current, bool eof = false)
            : m_parent_of_sub_root(parent_of_sub_root), m_current(current), m_eof(eof)
        {
            assert( (m_eof && NULL == m_current) ||
                (m_eof && NULL != m_current && NULL == m_current->getRightChild()) ||
                (!m_eof && NULL != m_current));
        }

        NODE& operator*() const
        {
            assert(!m_eof && NULL != m_current);
            return *m_current;
        }

        NODE* operator->() const
        {
            assert(!m_eof && NULL != m_current);
            return m_current;
        }

        InorderTraversalIterator& operator++()
        {
            assert(NULL != m_current);
            assert(!m_eof); // no next
            if (NULL != m_current->getRightChild())
            {
                m_current = m_current->getRightChild();
                while (NULL != m_current->getLeftChild())
                    m_current = m_current->getLeftChild();
            }
            else
            {
                NODE *parent = m_current->getParent();
                while (m_parent_of_sub_root != parent && m_current == parent->getRightChild())
                {
                    m_current = parent;
                    parent = m_current->getParent();
                }
                if (m_parent_of_sub_root == parent)
                    m_eof = true; // end
                else
                    m_current = parent;
            }
            return *this;
        }

        InorderTraversalIterator& operator--()
        {
            assert(NULL != m_current);
            if (m_eof)
            {
                m_eof = false;
            }
            else if (NULL != m_current->getLeftChild())
            {
                m_current = m_current->getLeftChild();
                while (NULL != m_current->getRightChild())
                    m_current = m_current->getRightChild();
            }
            else
            {
                NODE *parent = m_current->getParent();
                while (m_parent_of_sub_root != parent && m_current = parent->getLeftChild())
                {
                    m_current = parent;
                    parent = m_current->getParent();
                }
                assert(m_parent_of_sub_root != parent); // no pre
                m_current = parent;
            }
            return *this;
        }

        InorderTraversalIterator operator++(int)
        {
            InorderTraversalIterator ret = *this;
            ++*this;
            return ret;
        }

        InorderTraversalIterator operator--(int)
        {
            InorderTraversalIterator ret = *this;
            --*this;
            return ret;
        }

        bool operator==(const InorderTraversalIterator& i) const
        {
            return m_parent_of_sub_root == i->m_parent_of_sub_root &&
                m_current == i.m_current && m_eof = i.m_eof;
        }

        bool operator!=(const InorderTraversalIterator& i) const
        {
            return !(*this == i);
        }
    };

    /** 中序遍历的起始迭代器 */
    static InorderTraversalIterator inorder_traversal_begin(NODE *sub_root)
    {
        if (NULL == sub_root)
            return InorderTraversalIterator(NULL, NULL, true);

        const parent_of_sub_root = sub_root->getParent();
        while (NULL != sub_root->getLeftChild())
            sub_root = sub_root->getLeftChild();
        return InorderTraversalIterator(parent_of_sub_root, sub_root, false);
    }

    /** 中序遍历的终止迭代器 */
    static InorderTraversalIterator inorder_traversal_end(NODE *sub_root)
    {
        if (NULL == sub_root)
            return InorderTraversalIterator(NULL, NULL, true);

        const parent_of_sub_root = sub_root->getParent();
        while (NULL != sub_root->getRightChild())
            sub_root = sub_root->getRightChild();
        return InorderTraversalIterator(parent_of_sub_root, sub_root, true);
    }

    /**
     * 前序遍历迭代器
     *
     *        A
     *       / \
     *      B   C
     */
    class PreorderTraversalIterator
    {
        const NODE *m_parent_of_sub_root;
        NODE *m_current;
        bool m_eof;

    public:
        PreorderTraversalIterator(NODE *parent_of_sub_root, NODE *current, bool eof = false)
            : m_parent_of_sub_root(parent_of_sub_root), m_current(current), m_eof(eof)
        {
            assert( (m_eof && NULL == m_current) ||
                (m_eof && NULL != m_current && NULL == m_current->getLeftChild() && NULL == m_current->getRightChild()) ||
                (!m_eof && NULL != m_current) );
        }

        NODE& operator*() const
        {
            assert(NULL != m_current);
            return *m_current;
        }

        NODE* operator->() const 
        { 
            assert(NULL != m_current);
            return m_current;
        }

        PreorderTraversalIterator& operator++()
        {
            assert(NULL != m_current);
            assert(!m_eof); // no next
            if (NULL != m_current->getLeftChild())
            {
                m_current = m_current->getLeftChild();
            }
            else if (NULL != m_current->getRightChild())
            {
                m_current = m_current->getRightChild();
            }
            else
            {
                NODE *parent = m_current->getParent();
                while (m_parent_of_sub_root != parent &&
                    (m_current == parent->getRightChild() || NULL == parent->getRightChild()))
                {
                    m_current = parent;
                    parent = m_current->getParent();
                }
                if (m_parent_of_sub_root == parent)
                    m_eof = true; // end
                else
                    m_current = parent->getRightChild();
            }
            return *this;
        }

        PreorderTraversalIterator& operator--()
        {
            assert(NULL != m_current);
            if (m_eof)
            {
                m_eof = false;
            }
            else
            {
                NODE *parent = m_current->getParent();
                assert(m_parent_of_sub_root != parent); // no pre
                if (m_current == parent->getLeftChild() || NULL == parent->getLeftChild())
                {
                    m_current = parent;
                }
                else
                {
                    m_current = parent->getLeftChild();
                    while (true)
                    {
                        if (NULL != m_current->getRightChild())
                            m_current = m_current->getRightChild();
                        else if (NULL != m_current->getLeftChild())
                            m_current = m_current->getLeftChild();
                        else
                            break;
                    }
                }
            }
            return *this;
        }

        PreorderTraversalIterator operator++(int)
        {
            PreorderTraversalIterator ret = *this;
            ++*this;
            return ret;
        }

        PreorderTraversalIterator operator--(int)
        {
            PreorderTraversalIterator ret = *this;
            --*this;
            return ret;
        }

        bool operator==(const PreorderTraversalIterator& i) const
        {
            return m_parent_of_sub_root == i.m_parent_of_sub_root &&
                m_current == i.m_current && m_eof == i.m_eof;
        }

        bool operator!=(const PreorderTraversalIterator& i) const
        {
            return !(*this == i);
        }
    };

    /** 前序遍历的起始迭代器 */
    static PreorderTraversalIterator preorder_traversal_begin(NODE *sub_root)
    {
        if (NULL == sub_root)
            return PreorderTraversalIterator(NULL, NULL, true);
        return PreorderTraversalIterator(sub_root->getParent(), sub_root, false);
    }

    /** 前序遍历的终止迭代器 */
    static PreorderTraversalIterator preorder_traversal_end(NODE *sub_root)
    {
        if (NULL == sub_root)
            return PreorderTraversalIterator(NULL, NULL, true);

        const NODE* parent_of_sub_root = sub_root->getParent();
        while (NULL != sub_root->getRightChild())
            sub_root = sub_root->getRightChild();
        return PreorderTraversalIterator(parent_of_sub_root, sub_root, true);
    }

    /**
     * 后序遍历迭代器
     *
     *        C
     *       / \
     *      A   B
     */
    class PostorderTraversalIterator
    {
        const NODE *m_parent_of_sub_root;
        NODE *m_current;
        bool m_eof;

    public:
        PostorderTraversalIterator(NODE *parent_of_sub_root, NODE *current, bool eof = false)
            : m_parent_of_sub_root(parent_of_sub_root), m_current(current), m_eof(eof)
        {
            assert( (m_eof && NULL == m_current) ||
                (m_eof && NULL != m_current && m_parent_of_sub_root == m_current->getParent()) ||
                (!m_eof && NULL != m_current) );
        }

        NODE& operator*() const
        {
            assert(NULL != m_current);
            return *m_current;
        }

        NODE* operator->() const
        {
            assert(NULL != m_current);
            return m_current;
        }

        PostorderTraversalIterator& operator++()
        {
            assert(NULL != m_current);
            assert(!m_eof); // no next
            NODE *parent = m_current->getParent();
            if (m_parent_of_sub_root == parent)
            {
                m_eof = true; // end
            }
            else if (m_current == parent->getRightChild() || NULL == parent->getRightChild())
            {
                m_current = parent;
            }
            else
            {
                m_current = parent.getRightChild();
                while (true)
                {
                    if (NULL != m_current->getLeftChild())
                        m_current = m_current->getLeftChild();
                    else if (NULL != m_current->getRightChild())
                        m_current = m_current->getRightChild();
                    else
                        break;
                }
            }

            return *this;
        }

        PostorderTraversalIterator& operator--()
        {
            assert(NULL != m_current);
            if (m_eof)
            {
                m_eof = false;
            }
            else if (NULL != m_current->getRightChild())
            {
                m_current = m_current->getRightChild();
            }
            else if (NULL != m_current->getLeftChild())
            {
                m_current = m_current->getLeftChild();
            }
            else
            {
                NODE *parent = m_current->getParent();
                while (m_parent_of_sub_root != parent &&
                    (m_current == parent->getLeftChild() || NULL == m_current->getLeftChild()))
                {
                    m_current = parent;
                    parent = m_current->getParent();
                }
                assert(m_parent_of_sub_root != parent); // no pre
                m_current = parent->getLeftChild();
            }
            return *this;
        }

        PostorderTraversalIterator operator++(int)
        {
            PostorderTraversalIterator ret = *this;
            ++*this;
            return ret;
        }

        PostorderTraversalIterator operator--(int)
        {
            PostorderTraversalIterator ret = *this;
            --*this;
            return ret;
        }

        bool operator==(const PostorderTraversalIterator& i) const
        {
            return m_parent_of_sub_root == i.m_parent_of_sub_root &&
                m_current == i.m_current && m_eof == i.m_eof;
        }

        bool operator!=(const PostorderTraversalIterator& i) const
        {
            return !(*this == i);
        }
    };

    /** 后序遍历的起始迭代器 */
    static PostorderTraversalIterator postorder_traversal_begin(NODE *sub_root)
    {
        if (NULL == sub_root)
            return PostorderTraversalIterator(NULL, NULL, true);

        const NODE* parent_of_sub_root = sub_root->getParent();
        while (NULL != sub_root->getLeftChild())
            sub_root = sub_root->getLeftChild();
        return PostorderTraversalIterator(parent_of_sub_root, sub_root, false);
    }

    /** 后序遍历的终止迭代器 */
    static PostorderTraversalIterator postorder_traversal_end(NODE *sub_root)
    {
        if (NULL == sub_root)
            return PostorderTraversalIterator(NULL, NULL, true);
        return PostorderTraversalIterator(sub_root->getParent(), sub_root, true);
    }
};

}

#endif /* head file guarder */

