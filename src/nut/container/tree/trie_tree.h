
#ifndef ___HEADFILE_070AF0D8_976A_429F_AEE3_0D8B54BF7C8F_
#define ___HEADFILE_070AF0D8_976A_429F_AEE3_0D8B54BF7C8F_

#include <assert.h>
#include <stdlib.h> // for ::malloc(), ::free()
#include <algorithm> // for std::reverse()
#include <stack>
#include <vector>

#include "../../platform/int_type.h" // for ssize_t in Windows
#include "../comparable.h"
#include "binary_tree.h"
#include "rbtree.h"


namespace nut
{

/**
 * 字典树
 *
 * 例如: "a/d"、"b"、"b/e"、"b/f/g"、"c" 这5个路径组成了如下的字典树：
 *
 *          -
 *       /  |  \
 *      a  [b] [c]
 *     /   / \
 *   [d] [e]  f
 *             \
 *             [g]
 *
 * NOTE 为了平衡插入、查找速度，这里将字典树节点的子节点也组织成一颗红黑树。
 *      例如, 上面例子中根节点('-'节点)的子节点 a、b、c 组成红黑树, b 节点的子节
 *      点 e、f 组成另一颗红黑树.... 依此类推
 */
template <typename ENTRY, typename DATA>
class TrieTree
{
private:
    /**
     * 字典树中的节点, 同时其子节点组成一颗红黑树
     */
    class Node
    {
    public:
        void construct_plump(ENTRY&& entry, DATA&& data)
        {
            construct_transit(std::forward<ENTRY>(entry));

            _has_data = true;
            new (&_data) DATA(std::forward<DATA>(data));
        }

        void construct_plump(const ENTRY& entry, DATA&& data)
        {
            construct_transit(entry);

            _has_data = true;
            new (&_data) DATA(std::forward<DATA>(data));
        }

        void construct_plump(ENTRY&& entry, const DATA& data)
        {
            construct_transit(std::forward<ENTRY>(entry));

            _has_data = true;
            new (&_data) DATA(data);
        }

        void construct_plump(const ENTRY& entry, const DATA& data)
        {
            construct_transit(entry);

            _has_data = true;
            new (&_data) DATA(data);
        }

        void construct_transit(ENTRY&& entry)
        {
            construct_dummy();

            _has_entry = true;
            new (const_cast<ENTRY*>(&_entry)) ENTRY(std::forward<ENTRY>(entry));
        }

        void construct_transit(const ENTRY& entry)
        {
            construct_dummy();

            _has_entry = true;
            new (const_cast<ENTRY*>(&_entry)) ENTRY(entry);
        }

        void construct_dummy()
        {
            _has_entry = false;
            _has_data = false;

            _trie_parent = nullptr;
            _rb_root = nullptr;

            _rb_red = false;
            _rb_parent = nullptr;
            _rb_left = nullptr;
            _rb_right = nullptr;
        }

        void destruct()
        {
            if (_has_entry)
                (&_entry)->~ENTRY();
            if (_has_data)
                (&_data)->~DATA();

            clear_trie_children();
        }

        const ENTRY& get_entry() const
        {
            return _entry;
        }

        bool has_data() const
        {
            return _has_data;
        }

        const DATA& get_data() const
        {
            assert(_has_data);
            return _data;
        }

        void set_data(DATA&& data)
        {
            if (_has_data)
                _data = std::forward<DATA>(data);
            else
                new (&_data) DATA(std::forward<DATA>(data));
            _has_data = true;
        }

        void set_data(const DATA& data)
        {
            if (_has_data)
                _data = data;
            else
                new (&_data) DATA(data);
            _has_data = true;
        }

        void clear_data()
        {
            if (!_has_data)
                return;
            (&_data)->~DATA();
            _has_data = false;
        }

        std::vector<ENTRY> get_path() const
        {
            std::vector<ENTRY> path;
            Node *n = this;
            while (nullptr != n && n->_has_entry)
            {
                path.push_back(n->_entry);
                n = n->_trie_parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        Node* get_trie_parent() const
        {
            return _trie_parent;
        }

        Node* get_trie_child(const ENTRY& entry) const
        {
            return BSTree<ENTRY,Node>::search(_rb_root, entry);
        }

        Node* get_trie_child_tree() const
        {
            return _rb_root;
        }

        void add_trie_child(Node *n)
        {
            assert(n->_has_entry);
            _rb_root = RBTree<ENTRY,Node>::insert(_rb_root, n);
            n->_trie_parent = this;
        }

        void remove_trie_child(Node *n)
        {
            assert(n->_has_entry);
            _rb_root = RBTree<ENTRY,Node>::remove(_rb_root, n);
            n->_trie_parent = nullptr;
        }

        bool is_trie_leaf() const
        {
            return nullptr == _rb_root;
        }

        void clear_trie_children()
        {
            if (nullptr == _rb_root)
                return;

            // 遍历红黑树子节点、字典树子节点
            std::stack<Node*> s;
            s.push(_rb_root);
            while (!s.empty())
            {
                Node *n = s.top();
                assert(nullptr != n);
                s.pop();

                if (nullptr != n->_rb_left)
                    s.push(n->_rb_left);
                if (nullptr != n->_rb_right)
                    s.push(n->_rb_right);
                if (nullptr != n->_rb_root)
                    s.push(n->_rb_root);

                // NOTE 避免 destruct() 递归调用 clear_trie_children() 时重复析构
                n->_rb_left = nullptr;
                n->_rb_right = nullptr;
                n->_rb_root = nullptr;

                n->destruct();
                ::free(n);
            }
            _rb_root = nullptr;
        }

        size_t count_of_data() const
        {
            size_t ret = 0;

            std::stack<const Node*> s;
            s.push(this);
            while (!s.empty())
            {
                const Node *n = s.top();
                assert(nullptr != n);
                s.pop();

                if (n->_has_data)
                    ++ret;

                for (auto iter = BinaryTree<Node>::inorder_traversal_begin(n->_rb_root),
                         end = BinaryTree<Node>::inorder_traversal_end(n->_rb_root);
                     iter != end; ++iter)
                {
                    Node *child = &*iter;
                    assert(nullptr != child);
                    s.push(child);
                }
            }

            return ret;
        }

    private:
        // 红黑树接口
        friend class BinaryTree<Node>;
        friend class BSTree<ENTRY,Node>;
        friend class RBTree<ENTRY,Node>;

        const ENTRY& get_key() const
        {
            assert(_has_entry);
            return _entry;
        }

        bool is_red() const
        {
            return _rb_red;
        }

        Node* get_parent() const
        {
            return _rb_parent;
        }

        Node* get_left_child() const
        {
            return _rb_left;
        }

        Node* get_right_child() const
        {
            return _rb_right;
        }

        void set_red(bool red)
        {
            _rb_red = red;
        }

        void set_parent(Node* n)
        {
            _rb_parent = n;
        }

        void set_left_child(Node *n)
        {
            _rb_left = n;
        }

        void set_right_child(Node *n)
        {
            _rb_right = n;
        }

    private:
        Node() = delete;
        Node(const Node&) = delete;
        ~Node() = delete;
        Node& operator=(const Node&) = delete;

    public:
        bool _has_entry = true;
        const ENTRY _entry;

        bool _has_data = true;
        DATA _data;

        // 字典树属性
        Node *_trie_parent = nullptr, *_rb_root = nullptr; // 子节点构成一颗红黑树

        // 红黑树属性
        bool _rb_red = false;
        Node *_rb_parent = nullptr, *_rb_left = nullptr, *_rb_right = nullptr;
    };

public:
    TrieTree() = default;

    ~TrieTree()
    {
        clear();
    }

    size_t size() const
    {
        return _size;
    }

    /**
     * @return true, 插入成功
     *         false, 存在重复 path, 插入失败
     */
    bool insert(const ENTRY *path, size_t path_len, DATA&& data)
    {
        assert(nullptr != path && path_len > 0);
        return 0 != put(path, path_len, std::forward<DATA>(data), false);
    }

    /**
     * @return true, 插入成功
     *         false, 存在重复 path, 插入失败
     */
    bool insert(const ENTRY *path, size_t path_len, const DATA& data)
    {
        assert(nullptr != path && path_len > 0);
        return 0 != put(path, path_len, data, false);
    }

    /**
     * @return -1, duplicated path, force replaced
     *         0, duplicated path, canceled
     *         1, new data inserted
     */
    int put(const ENTRY *path, size_t path_len, DATA&& data, bool force = true)
    {
        assert(nullptr != path && path_len > 0);
        Node *n = ensure_path(path, path_len);
        assert(nullptr != n);
        if (!force && n->has_data())
            return 0;

        const int ret = (n->has_data() ? -1 : 1);
        if (1 == ret)
            ++_size;
        n->set_data(std::forward<DATA>(data));
        return ret;
    }

    /**
     * @return -1, duplicated path, force replaced
     *         0, duplicated path, canceled
     *         1, new data inserted
     */
    bool put(const ENTRY *path, size_t path_len, const DATA& data, bool force = true)
    {
        assert(nullptr != path && path_len > 0);
        Node *n = ensure_path(path, path_len);
        assert(nullptr != n);
        if (!force && n->has_data())
            return 0;

        const int ret = (n->has_data() ? -1 : 1);
        if (1 == ret)
            ++_size;
        n->set_data(data);
        return ret;
    }

    /**
     * 删除数据
     */
    bool remove(const ENTRY *path, size_t path_len)
    {
        assert(nullptr != path || 0 == path_len);
        Node *n = find_path(path, path_len);
        if (nullptr == n || !n->has_data())
            return false;

        n->clear_data();
        strip_branch(n);
        --_size;
        return true;
    }

    /**
     * 删除子树(包含指定的节点自身)
     *
     * @return 删除的数据数
     */
    size_t remove_tree(const ENTRY *path, size_t path_len)
    {
        assert(nullptr != path || 0 == path_len);

        if (0 == path_len)
        {
            const size_t ret = _size;
            clear();
            return ret;
        }

        Node *n = find_path(path, path_len);
        if (nullptr == n)
            return 0;

        const size_t ret = n->count_of_data();
        n->clear_trie_children();
        n->clear_data();
        strip_branch(n);
        _size -= ret;
        return ret;
    }

    /**
     * 获取数据
     */
    const DATA* get(const ENTRY *path, size_t path_len) const
    {
        assert(nullptr != path || 0 == path_len);
        const Node *n = find_path(path, path_len);
        if (nullptr == n || !n->has_data())
            return nullptr;
        return &n->get_data();
    }

    /**
     * 获取子孙节点中的数据(包含指定的节点自身)
     */
    std::vector<DATA> get_descendants(const ENTRY *path = nullptr, size_t path_len = 0) const
    {
        assert(nullptr != path || 0 == path_len);
        std::vector<DATA> ret;
        const Node *n = find_path(path, path_len);
        if (path_len > 0 && nullptr == n)
            return ret;

        std::stack<const Node*> s;
        s.push(n);
        while (!s.empty())
        {
            n = s.top();
            s.pop();

            if (nullptr != n && n->has_data())
                ret.push_back(n->get_data());

            Node *const root = (nullptr == n ? _rb_root : n->get_trie_child_tree());
            for (auto iter = BinaryTree<Node>::inorder_traversal_rbegin(root),
                     end = BinaryTree<Node>::inorder_traversal_rend(root);
                 iter != end; ++iter)
            {
                Node *const child = &*iter;
                assert(nullptr != child);
                s.push(child);
            }
        }
        return ret;
    }

    /**
     * 获取先祖节点中的数据(包含指定的节点自身)
     */
    std::vector<DATA> get_ancestors(const ENTRY *path, size_t path_len) const
    {
        assert(nullptr != path || 0 == path_len);
        const Node *n = find_path(path, path_len, true);

        std::vector<DATA> ret;
        while (nullptr != n)
        {
            if (n->has_data())
                ret.push_back(n->get_data());
            n = n->get_trie_parent();
        }
        return ret;
    }

    /**
     * 清除所有数据
     */
    void clear()
    {
        if (nullptr == _rb_root)
            return;

        BinaryTree<Node>::delete_tree(_rb_root, [] (Node *n) {
            n->destruct();
            ::free(n);
        });
        _rb_root = nullptr;
        _size = 0;
    }

private:
    TrieTree(const TrieTree&) = delete;
    TrieTree& operator=(const TrieTree&) = delete;

    /**
     * @param ensure   如果路径不存在, 则创建
     * @param accestor 如果路径不存在, 且 ensure 为 false, 则返回父节点
     */
    Node* ensure_path(const ENTRY *path, size_t path_len)
    {
        assert(nullptr != path || 0 == path_len);

        Node *n = nullptr;
        for (size_t i = 0; i < path_len; ++i)
        {
            assert(0 == i || nullptr != n);
            Node *child;
            if (0 == i)
                child = BSTree<ENTRY,Node>::search(_rb_root, path[i]);
            else
                child = n->get_trie_child(path[i]);

            if (nullptr != child)
            {
                n = child;
                continue;
            }

            Node *new_node = (Node*) ::malloc(sizeof(Node));
            new_node->construct_transit(path[i]);
            if (0 == i)
                _rb_root = RBTree<ENTRY,Node>::insert(_rb_root, new_node);
            else
                n->add_trie_child(new_node);
            n = new_node;
        }
        return n;
    }

    /**
     * @param accestor 如果路径不存在, 返回缺失节点的父节点而不是 nullptr
     */
    const Node* find_path(const ENTRY *path, size_t path_len, bool ancestor = false) const
    {
        assert(nullptr != path || 0 == path_len);

        const Node *n = nullptr;
        for (size_t i = 0; i < path_len; ++i)
        {
            assert(0 == i || nullptr != n);
            const Node *child;
            if (0 == i)
                child = BSTree<ENTRY,Node>::search(_rb_root, path[i]);
            else
                child = n->get_trie_child(path[i]);

            if (nullptr != child)
            {
                n = child;
                continue;
            }

            return ancestor ? n : nullptr;
        }
        return n;
    }

    Node* find_path(const ENTRY *path, size_t path_len, bool ancestor = false)
    {
        assert(nullptr != path || 0 == path_len);
        return const_cast<Node*>(
            static_cast<const TrieTree<ENTRY,DATA>*>(this)->find_path(
                path, path_len, ancestor));
    }

    /**
     * 从指定节点到其先祖，剪除无用节点(自身没有数据, 子孙节点也没有数据的节点)
     */
    void strip_branch(Node *n)
    {
        while (nullptr != n && n->is_trie_leaf() && !n->has_data())
        {
            Node *const parent = n->get_trie_parent();
            if (nullptr == parent)
                _rb_root = RBTree<ENTRY,Node>::remove(_rb_root, n);
            else
                parent->remove_trie_child(n);
            n->destruct();
            ::free(n);
            n = parent;
        }
    }

public:
    Node *_rb_root = nullptr;
    size_t _size = 0;
};

}

#endif
