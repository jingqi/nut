/**
 * @file -
 * @author jingqi
 * @date 2012-03-02
 * @last-edit 2012-03-02 21:31:10 jingqi
 */

#ifndef ___HEADFILE_BDA2D5F4_E926_4BBB_A415_DE4463C47D1A_
#define ___HEADFILE_BDA2D5F4_E926_4BBB_A415_DE4463C47D1A_

#include <assert.h>
#include <iterator>

namespace nut
{

/**
 * 二叉树
 */
template <typename NODE>
class BTree
{
private:
    /**
     * 中序遍历迭代器
     *
     *        B
     *       / \
     *      A   C
     */
    class InorderTraversalIterator
    {
        const NODE * const m_parent_of_sub_root;
        NODE *m_current;
        bool m_eof;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef NODE                            value_type;
        typedef ptrdiff_t                       difference_type;
        typedef NODE                            reference;
        typedef NODE*                           pointer;

    public:
        InorderTraversalIterator(const NODE *parent_of_sub_root, NODE *current, bool eof = false)
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
                NODE *current = m_current, *parent = m_current->getParent();
                while (m_parent_of_sub_root != parent && current == parent->getRightChild())
                {
                    current = parent;
                    parent = current->getParent();
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
                while (m_parent_of_sub_root != parent && m_current == parent->getLeftChild())
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
            return m_parent_of_sub_root == i.m_parent_of_sub_root &&
                m_current == i.m_current && m_eof == i.m_eof;
        }

        bool operator!=(const InorderTraversalIterator& i) const
        {
            return !(*this == i);
        }
    };

public:
    typedef InorderTraversalIterator inorder_iterator;
    typedef std::reverse_iterator<inorder_iterator> inorder_reverse_iterator;

    /** 中序遍历的起始迭代器 */
    static inorder_iterator inorder_traversal_begin(NODE *sub_root)
    {
        if (NULL == sub_root)
            return inorder_iterator(NULL, NULL, true);

        NODE *parent_of_sub_root = sub_root->getParent();
        while (NULL != sub_root->getLeftChild())
            sub_root = sub_root->getLeftChild();
        return inorder_iterator(parent_of_sub_root, sub_root, false);
    }

    /** 中序遍历的终止迭代器 */
    static inorder_iterator inorder_traversal_end(NODE *sub_root)
    {
        if (NULL == sub_root)
            return inorder_iterator(NULL, NULL, true);

        NODE *parent_of_sub_root = sub_root->getParent();
        while (NULL != sub_root->getRightChild())
            sub_root = sub_root->getRightChild();
        return inorder_iterator(parent_of_sub_root, sub_root, true);
    }

    /** 逆中序遍历的终止迭代器 */
    static inorder_reverse_iterator inorder_traversal_rbegin(NODE *sub_root)
    {
        return inorder_reverse_iterator(inorder_traversal_end(sub_root));
    }

    /** 逆中序遍历的终止迭代器 */
    static inorder_reverse_iterator inorder_traversal_rend(NODE *sub_root)
    {
        return inorder_reverse_iterator(inorder_traversal_begin(sub_root));
    }

private:
    /**
     * 前序遍历迭代器
     *
     *        A
     *       / \
     *      B   C
     */
    class PreorderTraversalIterator
    {
        const NODE * const m_parent_of_sub_root;
        NODE *m_current;
        bool m_eof;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef NODE                            value_type;
        typedef ptrdiff_t                       difference_type;
        typedef NODE                            reference;
        typedef NODE*                           pointer;

    public:
        PreorderTraversalIterator(const NODE *parent_of_sub_root, NODE *current, bool eof = false)
            : m_parent_of_sub_root(parent_of_sub_root), m_current(current), m_eof(eof)
        {
            assert( (m_eof && NULL == m_current) ||
                (m_eof && NULL != m_current && NULL == m_current->getLeftChild() && NULL == m_current->getRightChild()) ||
                (!m_eof && NULL != m_current) );
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
                NODE *current = m_current, *parent = m_current->getParent();
                while (m_parent_of_sub_root != parent &&
                    (current == parent->getRightChild() || NULL == parent->getRightChild()))
                {
                    current = parent;
                    parent = current->getParent();
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

public:
    typedef PreorderTraversalIterator preorder_iterator;
    typedef std::reverse_iterator<preorder_iterator> preorder_reverse_iterator;

    /** 前序遍历的起始迭代器 */
    static preorder_iterator preorder_traversal_begin(NODE *sub_root)
    {
        if (NULL == sub_root)
            return preorder_iterator(NULL, NULL, true);
        return preorder_iterator(sub_root->getParent(), sub_root, false);
    }

    /** 前序遍历的终止迭代器 */
    static preorder_iterator preorder_traversal_end(NODE *sub_root)
    {
        if (NULL == sub_root)
            return preorder_iterator(NULL, NULL, true);

        const NODE* parent_of_sub_root = sub_root->getParent();
        while (NULL != sub_root->getRightChild())
            sub_root = sub_root->getRightChild();
        return preorder_iterator(parent_of_sub_root, sub_root, true);
    }

    /** 逆前序遍历的终止迭代器 */
    static preorder_reverse_iterator preorder_traversal_rbegin(NODE *sub_root)
    {
        return preorder_reverse_iterator(preorder_traversal_end(sub_root));
    }

    /** 逆前序遍历的终止迭代器 */
    static preorder_reverse_iterator preorder_traversal_rend(NODE *sub_root)
    {
        return preorder_reverse_iterator(preorder_traversal_begin(sub_root));
    }

private:
    /**
     * 后序遍历迭代器
     *
     *        C
     *       / \
     *      A   B
     */
    class PostorderTraversalIterator
    {
        const NODE * const m_parent_of_sub_root;
        NODE *m_current;
        bool m_eof;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef NODE                            value_type;
        typedef ptrdiff_t                       difference_type;
        typedef NODE                            reference;
        typedef NODE*                           pointer;

    public:
        PostorderTraversalIterator(const NODE *parent_of_sub_root, NODE *current, bool eof = false)
            : m_parent_of_sub_root(parent_of_sub_root), m_current(current), m_eof(eof)
        {
            assert( (m_eof && NULL == m_current) ||
                (m_eof && NULL != m_current && m_parent_of_sub_root == m_current->getParent()) ||
                (!m_eof && NULL != m_current) );
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
                m_current = parent->getRightChild();
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
                    (m_current == parent->getLeftChild() || NULL == parent->getLeftChild()))
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

public:
    typedef PostorderTraversalIterator postorder_iterator;
    typedef std::reverse_iterator<postorder_iterator> postorder_reverse_iterator;

    /** 后序遍历的起始迭代器 */
    static postorder_iterator postorder_traversal_begin(NODE *sub_root)
    {
        if (NULL == sub_root)
            return postorder_iterator(NULL, NULL, true);

        NODE* parent_of_sub_root = sub_root->getParent();
        while (NULL != sub_root->getLeftChild())
            sub_root = sub_root->getLeftChild();
        return postorder_iterator(parent_of_sub_root, sub_root, false);
    }

    /** 后序遍历的终止迭代器 */
    static postorder_iterator postorder_traversal_end(NODE *sub_root)
    {
        if (NULL == sub_root)
            return postorder_iterator(NULL, NULL, true);
        return postorder_iterator(sub_root->getParent(), sub_root, true);
    }

    /** 逆后序遍历的起始迭代器 */
    static postorder_reverse_iterator postorder_traversal_rbegin(NODE *sub_root)
    {
        return postorder_reverse_iterator(postorder_traversal_end(sub_root));
    }

    /** 逆后序遍历的终止迭代器 */
    static postorder_reverse_iterator postorder_traversal_rend(NODE *sub_root)
    {
        return postorder_reverse_iterator(postorder_traversal_begin(sub_root));
    }
};

}

#endif /* head file guarder */

