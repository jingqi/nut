
#ifndef ___HEADFILE_BDA2D5F4_E926_4BBB_A415_DE4463C47D1A_
#define ___HEADFILE_BDA2D5F4_E926_4BBB_A415_DE4463C47D1A_

#include <assert.h>
#include <stddef.h> // for ptrdiff_t
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
        const NODE *_parent_of_sub_root = NULL;
        NODE *_current = NULL;
        bool _eof = false;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef NODE                            value_type;
        typedef ptrdiff_t                       difference_type;
        typedef NODE&                           reference;
        typedef NODE*                           pointer;

    public:
        InorderTraversalIterator(const NODE *parent_of_sub_root, NODE *current, bool eof = false)
            : _parent_of_sub_root(parent_of_sub_root), _current(current), _eof(eof)
        {
            assert( (_eof && NULL == _current) ||
                (_eof && NULL != _current && NULL == _current->get_right_child()) ||
                (!_eof && NULL != _current));
        }

        NODE& operator*() const
        {
            assert(!_eof && NULL != _current);
            return *_current;
        }

        NODE* operator->() const
        {
            assert(!_eof && NULL != _current);
            return _current;
        }

        InorderTraversalIterator& operator++()
        {
            assert(NULL != _current);
            assert(!_eof); // no next
            if (NULL != _current->get_right_child())
            {
                _current = _current->get_right_child();
                while (NULL != _current->get_left_child())
                    _current = _current->get_left_child();
            }
            else
            {
                NODE *current = _current, *parent = _current->get_parent();
                while (_parent_of_sub_root != parent && current == parent->get_right_child())
                {
                    current = parent;
                    parent = current->get_parent();
                }
                if (_parent_of_sub_root == parent)
                    _eof = true; // end
                else
                    _current = parent;
            }
            return *this;
        }

        InorderTraversalIterator& operator--()
        {
            assert(NULL != _current);
            if (_eof)
            {
                _eof = false;
            }
            else if (NULL != _current->get_left_child())
            {
                _current = _current->get_left_child();
                while (NULL != _current->get_right_child())
                    _current = _current->get_right_child();
            }
            else
            {
                NODE *parent = _current->get_parent();
                while (_parent_of_sub_root != parent && _current == parent->get_left_child())
                {
                    _current = parent;
                    parent = _current->get_parent();
                }
                assert(_parent_of_sub_root != parent); // no pre
                _current = parent;
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
            return _parent_of_sub_root == i._parent_of_sub_root &&
                _current == i._current && _eof == i._eof;
        }

        bool operator!=(const InorderTraversalIterator& i) const
        {
            return !(*this == i);
        }
    };

public:
    typedef InorderTraversalIterator inorder_iterator;
    typedef std::reverse_iterator<inorder_iterator> inorder_reverse_iterator;

    /**
     * 中序遍历的起始迭代器
     */
    static inorder_iterator inorder_traversal_begin(NODE *sub_root)
    {
        if (NULL == sub_root)
            return inorder_iterator(NULL, NULL, true);

        NODE *parent_of_sub_root = sub_root->get_parent();
        while (NULL != sub_root->get_left_child())
            sub_root = sub_root->get_left_child();
        return inorder_iterator(parent_of_sub_root, sub_root, false);
    }

    /**
     * 中序遍历的终止迭代器
     */
    static inorder_iterator inorder_traversal_end(NODE *sub_root)
    {
        if (NULL == sub_root)
            return inorder_iterator(NULL, NULL, true);

        NODE *parent_of_sub_root = sub_root->get_parent();
        while (NULL != sub_root->get_right_child())
            sub_root = sub_root->get_right_child();
        return inorder_iterator(parent_of_sub_root, sub_root, true);
    }

    /**
     * 逆中序遍历的终止迭代器
     */
    static inorder_reverse_iterator inorder_traversal_rbegin(NODE *sub_root)
    {
        return inorder_reverse_iterator(inorder_traversal_end(sub_root));
    }

    /**
     * 逆中序遍历的终止迭代器
     */
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
        const NODE *_parent_of_sub_root = NULL;
        NODE *_current = NULL;
        bool _eof = false;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef NODE                            value_type;
        typedef ptrdiff_t                       difference_type;
        typedef NODE&                           reference;
        typedef NODE*                           pointer;

    public:
        PreorderTraversalIterator(const NODE *parent_of_sub_root, NODE *current, bool eof = false)
            : _parent_of_sub_root(parent_of_sub_root), _current(current), _eof(eof)
        {
            assert( (_eof && NULL == _current) ||
                (_eof && NULL != _current && NULL == _current->get_left_child() && NULL == _current->get_right_child()) ||
                (!_eof && NULL != _current) );
        }

        NODE& operator*() const
        {
            assert(!_eof && NULL != _current);
            return *_current;
        }

        NODE* operator->() const
        {
            assert(!_eof && NULL != _current);
            return _current;
        }

        PreorderTraversalIterator& operator++()
        {
            assert(NULL != _current);
            assert(!_eof); // no next
            if (NULL != _current->get_left_child())
            {
                _current = _current->get_left_child();
            }
            else if (NULL != _current->get_right_child())
            {
                _current = _current->get_right_child();
            }
            else
            {
                NODE *current = _current, *parent = _current->get_parent();
                while (_parent_of_sub_root != parent &&
                    (current == parent->get_right_child() || NULL == parent->get_right_child()))
                {
                    current = parent;
                    parent = current->get_parent();
                }
                if (_parent_of_sub_root == parent)
                    _eof = true; // end
                else
                    _current = parent->get_right_child();
            }
            return *this;
        }

        PreorderTraversalIterator& operator--()
        {
            assert(NULL != _current);
            if (_eof)
            {
                _eof = false;
            }
            else
            {
                NODE *parent = _current->get_parent();
                assert(_parent_of_sub_root != parent); // no pre
                if (_current == parent->get_left_child() || NULL == parent->get_left_child())
                {
                    _current = parent;
                }
                else
                {
                    _current = parent->get_left_child();
                    while (true)
                    {
                        if (NULL != _current->get_right_child())
                            _current = _current->get_right_child();
                        else if (NULL != _current->get_left_child())
                            _current = _current->get_left_child();
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
            return _parent_of_sub_root == i._parent_of_sub_root &&
                _current == i._current && _eof == i._eof;
        }

        bool operator!=(const PreorderTraversalIterator& i) const
        {
            return !(*this == i);
        }
    };

public:
    typedef PreorderTraversalIterator preorder_iterator;
    typedef std::reverse_iterator<preorder_iterator> preorder_reverse_iterator;

    /**
     * 前序遍历的起始迭代器
     */
    static preorder_iterator preorder_traversal_begin(NODE *sub_root)
    {
        if (NULL == sub_root)
            return preorder_iterator(NULL, NULL, true);
        return preorder_iterator(sub_root->get_parent(), sub_root, false);
    }

    /**
     * 前序遍历的终止迭代器
     */
    static preorder_iterator preorder_traversal_end(NODE *sub_root)
    {
        if (NULL == sub_root)
            return preorder_iterator(NULL, NULL, true);

        const NODE* parent_of_sub_root = sub_root->get_parent();
        while (NULL != sub_root->get_right_child())
            sub_root = sub_root->get_right_child();
        return preorder_iterator(parent_of_sub_root, sub_root, true);
    }

    /**
     * 逆前序遍历的终止迭代器
     */
    static preorder_reverse_iterator preorder_traversal_rbegin(NODE *sub_root)
    {
        return preorder_reverse_iterator(preorder_traversal_end(sub_root));
    }

    /**
     * 逆前序遍历的终止迭代器
     */
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
        const NODE *_parent_of_sub_root = NULL;
        NODE *_current = NULL;
        bool _eof = false;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef NODE                            value_type;
        typedef ptrdiff_t                       difference_type;
        typedef NODE&                           reference;
        typedef NODE*                           pointer;

    public:
        PostorderTraversalIterator(const NODE *parent_of_sub_root, NODE *current, bool eof = false)
            : _parent_of_sub_root(parent_of_sub_root), _current(current), _eof(eof)
        {
            assert( (_eof && NULL == _current) ||
                (_eof && NULL != _current && _parent_of_sub_root == _current->get_parent()) ||
                (!_eof && NULL != _current) );
        }

        NODE& operator*() const
        {
            assert(!_eof && NULL != _current);
            return *_current;
        }

        NODE* operator->() const
        {
            assert(!_eof && NULL != _current);
            return _current;
        }

        PostorderTraversalIterator& operator++()
        {
            assert(NULL != _current);
            assert(!_eof); // no next
            NODE *parent = _current->get_parent();
            if (_parent_of_sub_root == parent)
            {
                _eof = true; // end
            }
            else if (_current == parent->get_right_child() || NULL == parent->get_right_child())
            {
                _current = parent;
            }
            else
            {
                _current = parent->get_right_child();
                while (true)
                {
                    if (NULL != _current->get_left_child())
                        _current = _current->get_left_child();
                    else if (NULL != _current->get_right_child())
                        _current = _current->get_right_child();
                    else
                        break;
                }
            }

            return *this;
        }

        PostorderTraversalIterator& operator--()
        {
            assert(NULL != _current);
            if (_eof)
            {
                _eof = false;
            }
            else if (NULL != _current->get_right_child())
            {
                _current = _current->get_right_child();
            }
            else if (NULL != _current->get_left_child())
            {
                _current = _current->get_left_child();
            }
            else
            {
                NODE *parent = _current->get_parent();
                while (_parent_of_sub_root != parent &&
                    (_current == parent->get_left_child() || NULL == parent->get_left_child()))
                {
                    _current = parent;
                    parent = _current->get_parent();
                }
                assert(_parent_of_sub_root != parent); // no pre
                _current = parent->get_left_child();
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
            return _parent_of_sub_root == i._parent_of_sub_root &&
                _current == i._current && _eof == i._eof;
        }

        bool operator!=(const PostorderTraversalIterator& i) const
        {
            return !(*this == i);
        }
    };

public:
    typedef PostorderTraversalIterator postorder_iterator;
    typedef std::reverse_iterator<postorder_iterator> postorder_reverse_iterator;

    /**
     * 后序遍历的起始迭代器
     */
    static postorder_iterator postorder_traversal_begin(NODE *sub_root)
    {
        if (NULL == sub_root)
            return postorder_iterator(NULL, NULL, true);

        NODE* parent_of_sub_root = sub_root->get_parent();
        while (NULL != sub_root->get_left_child())
            sub_root = sub_root->get_left_child();
        return postorder_iterator(parent_of_sub_root, sub_root, false);
    }

    /**
     * 后序遍历的终止迭代器
     */
    static postorder_iterator postorder_traversal_end(NODE *sub_root)
    {
        if (NULL == sub_root)
            return postorder_iterator(NULL, NULL, true);
        return postorder_iterator(sub_root->get_parent(), sub_root, true);
    }

    /**
     * 逆后序遍历的起始迭代器
     */
    static postorder_reverse_iterator postorder_traversal_rbegin(NODE *sub_root)
    {
        return postorder_reverse_iterator(postorder_traversal_end(sub_root));
    }

    /**
     * 逆后序遍历的终止迭代器
     */
    static postorder_reverse_iterator postorder_traversal_rend(NODE *sub_root)
    {
        return postorder_reverse_iterator(postorder_traversal_begin(sub_root));
    }
};

}

#endif /* head file guarder */
