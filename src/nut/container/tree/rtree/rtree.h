
#ifndef ___HEADFILE_160547E9_5A30_4A78_A5FF_76E0C5EBE229_
#define ___HEADFILE_160547E9_5A30_4A78_A5FF_76E0C5EBE229_

#include <list>
#include <vector>
#include <stack>
#include <map> // for pair
#include <assert.h>

#include "../../../platform/platform.h"

#if NUT_PLATFORM_CC_VC
#   include <xutility> // for std::min() and so on
#endif

#include "mdarea.h"


namespace nut
{

/**
 * 多维空间 rtree
 *
 * 参考资料：
 *      R-Trees - A Dynamic index structure for spatial searching. Antonin Guttman.
 *  University of California Berkeley
 *
 * @param DATA_TYPE 数据类型; 可以是 int、void*, obj* 等
 * @param NUM_TYPE 数字类型; 可以是 int、float 等
 * @param DIMENSIONS 区域维数; 大于等于2
 * @param FLOAT_TYPE 实数，避免乘法运算溢出，常用的是float，double等
 * @param MAX_ENTRY_COUNT 节点最大孩子数; 大于等于2
 * @param MIN_ENTRY_COUNT 节点最小孩子数; 小于最大孩子数
 */
template <typename DATA_TYPE, typename NUM_TYPE, size_t DIMENSIONS = 2,
          typename FLOAT_TYPE = double, size_t MAX_ENTRY_COUNT = 5,
          size_t MIN_ENTRY_COUNT = MAX_ENTRY_COUNT / 2>
class RTree
{
    static_assert(DIMENSIONS >= 2, "rtree 空间维数");
    static_assert(MAX_ENTRY_COUNT >= 2, "rtree 最大子节点数");
    static_assert(1 <= MIN_ENTRY_COUNT && MIN_ENTRY_COUNT <= (MAX_ENTRY_COUNT + 1) / 2,
                  "rtree 最小子节点数");

public:
    typedef typename std::enable_if<std::is_integral<NUM_TYPE>::value ||
                                    std::is_floating_point<NUM_TYPE>::value,
                                    NUM_TYPE>::type num_type;

    typedef typename std::enable_if<std::is_floating_point<FLOAT_TYPE>::value,
                                    FLOAT_TYPE>::type float_type;

    typedef MDArea<num_type, DIMENSIONS, float_type> area_type;
    typedef DATA_TYPE data_type;

private:
    /**
     * 节点基类
     */
    class TreeNode;
    
    class Node
    {
    public:
        explicit Node(bool tn) noexcept
            : _tree_node(tn)
        {}

        Node(bool tn, const area_type& rt) noexcept
            : area(rt), _tree_node(tn) 
        {}

        virtual ~Node() = default;

        bool is_tree_node() const noexcept
        {
            return _tree_node;
        }

    public:
        area_type area;
        TreeNode *parent = nullptr;

    private:
        bool _tree_node = true; // 是树节点还是数据节点
    };

    /**
     * 树节点
     */
    class TreeNode : public Node
    {
    public:
        TreeNode() noexcept
            : Node(true)
        {
            ::memset(children, 0, sizeof(Node*) * MAX_ENTRY_COUNT);
        }

        Node* child_at(size_t i) noexcept
        {
            assert(i < MAX_ENTRY_COUNT);
            return children[i];
        }

        size_t child_count() const noexcept
        {
            // 二分查找
            int left = -1, right = MAX_ENTRY_COUNT;
            while (left + 1 < right)
            {
                int middle = (left + right) / 2;
                if (nullptr == children[middle])
                    right = middle;
                else
                    left = middle;
            }
            return (size_t) right;
        }

        bool append_child(Node *child, bool set_child_parent = true) noexcept
        {
            assert(nullptr != child);

            if (nullptr != children[MAX_ENTRY_COUNT - 1])
                return false;
            children[child_count()] = child;
            if (set_child_parent)
                child->parent = this; // 附带设置 parent
            return true;
        }

        bool remove_child(Node *child, bool set_child_parent = true) noexcept
        {
            assert(nullptr != child);
            for (size_t i = 0; i < MAX_ENTRY_COUNT && nullptr != children[i]; ++i)
            {
                if (children[i] == child)
                {
                    if (set_child_parent)
                        child->parent = nullptr; // 附带设置 parent
                    children[i] = nullptr;

                    // 保持紧凑
                    for (size_t j = i; j + 1 < MAX_ENTRY_COUNT && nullptr != children[j + 1]; ++j)
                    {
                        children[j] = children[j + 1];
                        children[j + 1] = nullptr;
                    }
                    return true;
                }
            }
            return false;
        }

        void clear_children(bool set_child_parent = true) noexcept
        {
            for (size_t i = 0; i < MAX_ENTRY_COUNT && nullptr != children[i]; ++i)
            {
                if (set_child_parent)
                    children[i]->parent = nullptr; // 附带设置 parent
                children[i] = nullptr;
            }
        }

        /**
         * 调整区域，使父节点区域恰好包含所有子节点区域
         */
        void fit_rect() noexcept
        {
            assert(nullptr != children[0]);
            area = children[0]->area;
            for (size_t i = 1; i < MAX_ENTRY_COUNT && nullptr != children[i]; ++i)
                area.expand_to_contain(children[i]->area);
        }

    public:
        using Node::area;
        Node *children[MAX_ENTRY_COUNT];
    };

    /**
     * 数据节点
     */
    class DataNode : public Node
    {
    public:
        explicit DataNode(data_type&& v) noexcept
            : Node(false), data(std::forward<data_type>(v))
        {}

        explicit DataNode(const data_type& v) noexcept
            : Node(false), data(v)
        {}

        DataNode(const area_type& rt, data_type&& v) noexcept
            : Node(false, rt), data(std::forward<data_type>(v))
        {}

        DataNode(const area_type& rt, const data_type& v) noexcept
            : Node(false, rt), data(v)
        {}

    public:
        data_type data;
    };

    typedef RTree<data_type, num_type, DIMENSIONS, float_type, MAX_ENTRY_COUNT,
                  MIN_ENTRY_COUNT> self_type;

public:
    RTree() noexcept
    {
        _root = (TreeNode*) ::malloc(sizeof(TreeNode));
        assert(nullptr != _root);
        new (_root) TreeNode();
        _height = 1;
    }

    RTree(RTree&& x) noexcept
    {
        _root = x._root;
        _height = x._height;
        _size = x._size;
        
        x._root = nullptr; // NOTE 该对象已经不能执行任何有效操作，只能等待析构了
        x._height = 0;
        x._size = 0;
    }

    RTree(const RTree& x) noexcept
    {
        _root = (TreeNode*) ::malloc(sizeof(TreeNode));
        assert(nullptr != _root);
        new (_root) TreeNode();
        _height = 1;

        *this = x;
    }

    ~RTree() noexcept
    {
        clear();
        if (nullptr != _root)
        {
            _root->~TreeNode();
            ::free(_root);
            _root = nullptr;
        }
    }

    RTree& operator=(RTree&& x) noexcept
    {
        if (this == &x)
            return *this;

        clear();
        if (nullptr != _root)
        {
            _root->~TreeNode();
            ::free(_root);
            _root = nullptr;
        }

        _root = x._root;
        _height = x._height;
        _size = x._size;
        
        x._root = nullptr; // NOTE 该对象已经不能执行任何有效操作，只能等待析构了
        x._height = 0;
        x._size = 0;

        return *this;
    }

    RTree& operator=(const RTree& x) noexcept
    {
        if (this == &x)
            return *this;

        clear();

        *_root = *x._root;
        _height = x._height;
        _size = x._size;

        std::stack<TreeNode*> s1, s2;
        s1.push(_root);
        s2.push(x._root);
        while (!s2.empty())
        {
            TreeNode *n1 = s1.top(), *n2 = s2.top();
            s1.pop();
            s2.pop();

            for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
            {
                Node *c2 = n2->child_at(i);
                if (nullptr == c2)
                    break;
                if (c2->is_tree_node())
                {
                    TreeNode *t2 = dynamic_cast<TreeNode*>(c2);

                    TreeNode *t1 = (TreeNode*) ::malloc(sizeof(TreeNode));
                    assert(nullptr != t1);
                    new (t1) TreeNode(*t2);

                    t1->parent = n1;
                    n1->children[i] = t1;

                    s1.push(t1);
                    s2.push(t2);
                }
                else
                {
                    DataNode *d2 = dynamic_cast<DataNode*>(c2);

                    DataNode *d1 = (DataNode*) ::malloc(sizeof(DataNode));
                    assert(nullptr != d1);
                    new (d1) DataNode(*d2);

                    d1->parent = n1;
                    n1->children[i] = d1;
                }
            }
        }

        return *this;
    }

    /**
     * 插入数据
     */
    void insert(const area_type& rect, data_type&& data) noexcept
    {
        DataNode *data_node = (DataNode*) ::malloc(sizeof(DataNode));
        assert(nullptr != data_node);
        new (data_node) DataNode(rect, std::forward<data_type>(data));
        insert(data_node, _height);
        ++_size;
    }

    /**
     * 插入数据
     */
    void insert(const area_type& rect, const data_type& data) noexcept
    {
        DataNode *data_node = (DataNode*) ::malloc(sizeof(DataNode));
        assert(nullptr != data_node);
        new (data_node) DataNode(rect, data);
        insert(data_node, _height);
        ++_size;
    }

    /**
     * 移除第一个与给定区域相同的数据
     */
    bool remove_first(const area_type& rect) noexcept
    {
        // 找到数据节点并删除数据
        DataNode *dn = find_first_data_node(rect);
        if (nullptr == dn)
            return false;
        assert(nullptr != dn->parent);
        TreeNode *l = dynamic_cast<TreeNode*>(dn->parent);
        assert(nullptr != l);
        bool rs = l->remove_child(dn);
        if (!rs)
            return false;

        // 释放内存
        dn->~DataNode();
        ::free(dn);

        // 调整树
        condense_tree(l);
        if (_root->child_count() == 1 && _root->child_at(0)->is_tree_node())
        {
            TreeNode *tobedel = _root;
            _root = dynamic_cast<TreeNode*>(_root->child_at(0));
            _root->parent = nullptr;
            --_height;

            // 释放内存
            tobedel->~TreeNode();
            ::free(tobedel);
        }
        --_size;
        return true;
    }

    /**
     * 移除指定的映射
     */
    bool remove(const area_type& rect, const data_type& data) noexcept
    {
        // 找到数据节点并删除数据
        DataNode *dn = find_data_node(rect, data);
        if (nullptr == dn)
            return false;
        assert(nullptr != dn->parent);
        TreeNode *l = dynamic_cast<TreeNode*>(dn->parent);
        assert(nullptr != l);
        bool rs = l->remove_child(dn);
        if (!rs)
            return false;

        // 释放内存
        dn->~DataNode();
        ::free(dn);

        // 调整树
        condense_tree(l);
        if (_root->child_count() == 1 && _root->child_at(0)->is_tree_node())
        {
            TreeNode *tobedel = _root;
            _root = dynamic_cast<TreeNode*>(_root->child_at(0));
            _root->parent = nullptr;
            --_height;

            // 释放内存
            tobedel->~TreeNode();
            ::free(tobedel);
        }
        --_size;
        return true;
    }

    /**
     * 清除所有数据
     */
    void clear() noexcept
    {
        if (nullptr == _root)
            return;

        // 清除所有
        std::stack<TreeNode*> s;
        s.push(_root);
        while (!s.empty())
        {
            TreeNode *n = s.top();
            s.pop();

            for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
            {
                Node *c = n->child_at(i);
                if (nullptr == c)
                    break;
                if (c->is_tree_node())
                {
                    TreeNode *tn = dynamic_cast<TreeNode*>(c);
                    s.push(tn);
                }
                else
                {
                    DataNode *dn = dynamic_cast<DataNode*>(c);
                    dn->~DataNode();
                    ::free(dn);
                }
            }
            if (n != _root)
            {
                n->~TreeNode();
                ::free(n);
            }
        }
        _root->area.clear();
        _root->clear_children(false);
        _height = 1;
        _size = 0;
    }

    /**
     * 查找与指定区域相交的数据
     */
    void search_intersect(
        const area_type& rect,
        std::vector<std::pair<area_type,data_type>> *appended) noexcept
    {
        assert(nullptr != appended);

        std::stack<TreeNode*> s;
        s.push(_root);
        while (!s.empty())
        {
            TreeNode *n = s.top();
            s.pop();

            for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
            {
                Node *c = n->child_at(i);
                if (nullptr == c)
                    break;
                if (!c->area.intersects(rect))
                    continue;

                if (c->is_tree_node())
                {
                    TreeNode *tn = dynamic_cast<TreeNode*>(c);
                    s.push(tn);
                }
                else
                {
                    DataNode *dn = dynamic_cast<DataNode*>(c);
                    appended->emplace_back(dn->area, dn->data);
                }
            }
        }
    }

    /**
     * 查找包含在指定区域内的数据
     */
    void search_contains(
        const area_type& rect,
        std::vector<std::pair<area_type, data_type> > *appended) noexcept
    {
        assert(nullptr != appended);

        std::stack<TreeNode*> s;
        s.push(_root);
        while (!s.empty())
        {
            TreeNode *n = s.top();
            s.pop();

            for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
            {
                Node *c = n->child_at(i);
                if (nullptr == c)
                    break;
                if (!c->area.intersects(rect))
                    continue;

                if (c->is_tree_node())
                {
                    TreeNode *tn = dynamic_cast<TreeNode*>(c);
                    s.push(tn);
                }
                else if (rect.contains(c->area))
                {
                    DataNode *dn = dynamic_cast<DataNode*>(c);
                    appended->push_back(dn->data);
                }
            }
        }
    }

    /**
     * 返回所有的数据
     */
    void get_all(std::vector<data_type> *appended) noexcept
    {
        assert(nullptr != appended);

        std::stack<TreeNode*> s;
        s.push(_root);
        while (!s.empty())
        {
            TreeNode *n = s.top();
            s.pop();

            for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
            {
                Node *c = n->child_at(i);
                if (nullptr == c)
                    break;
                if (c->is_tree_node())
                {
                    TreeNode *tn = dynamic_cast<TreeNode*>(c);
                    s.push(tn);
                }
                else
                {
                    DataNode *dn = dynamic_cast<DataNode*>(c);
                    appended->push_back(dn->data);
                }
            }
        }
    }

    /**
     * 元素个数
     */
    size_t size() const noexcept
    {
        return _size;
    }

    /**
     * 树高，大于等于1
     */
    size_t height() const noexcept
    {
        return _height;
    }

    /**
     * 检查 rtree 结构是否错误
     */
    bool is_valid(Node *e = nullptr, size_t depth = 1) noexcept
    {
        if (nullptr == e)
        {
            e = _root;
            depth = 1;
        }
        assert(nullptr != e && 1 <= depth && depth <= _height + 1);

        if (depth == _height + 1)
        {
            if (e->is_tree_node())
                return false; // wrong node type with depth
            return true;
        }

        if (!e->is_tree_node())
            return false; // wrong node type with depth

        TreeNode *n = dynamic_cast<TreeNode*>(e);
        const size_t cc = n->child_count();
        if (depth != 1 && cc < MIN_ENTRY_COUNT)
            return false; // under fill
        for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
        {
            Node *ee = n->child_at(i);
            if (nullptr == ee)
                break;
            if (!n->area.contains(ee->area))
                return false; // area error
            if (!is_valid(ee, depth + 1))
                return false;
        }

        return true;
    }

private:
    /**
     * 扩展到包容指定的区域所需要扩展的空间
     */
    static float_type acreage_needed(const area_type& x, const area_type& y) noexcept
    {
        float_type new_acr = 1;
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            new_acr *= std::max(x.higher[i], y.higher[i]) -
                std::min(x.lower[i], y.lower[i]);
        }
        return new_acr - x.acreage();
    }

    /**
     * 将节点插入深度为 depth 的位置
     */
    void insert(Node *node, size_t depth) noexcept
    {
        assert(nullptr != node);

        TreeNode *n = choose_node(node->area, depth);
        TreeNode *ll = n->append_child(node) ? nullptr : split_node(n, node);
        TreeNode *r = adjust_tree(n, ll);
        if (nullptr != r)
        {
            // new root
            TreeNode *nln = (TreeNode*) ::malloc(sizeof(TreeNode));
            assert(nullptr != nln);
            new (nln) TreeNode();
            nln->append_child(_root);
            nln->append_child(r);
            nln->fit_rect();
            _root = nln;
            ++_height;
        }
    }

    /**
     * 根据目标区域选区适合的节点
     */
    TreeNode* choose_node(const area_type& rect_to_add, size_t depth) noexcept
    {
        TreeNode *ret = _root;
        while (depth > 1)
        {
            TreeNode *nn = nullptr;

            // choose the least enlargement child
            float_type least = 0;
            for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
            {
                Node *e = ret->child_at(i);
                if (nullptr == e)
                    break;
                float_type el = acreage_needed(e->area, rect_to_add);
                if (0 == i || el < least)
                {
                    nn = dynamic_cast<TreeNode*>(e); // 因为 depth > 1，这里应当是成功的
                    assert(nullptr != nn);
                    least = el;
                }
            }
            ret = nn;
            --depth;
        }
        return ret;
    }

    /**
     * 拆分节点
     *
     * @param parent The parent node to add a child, (it is full now)
     * @param child The child to be added
     */
    TreeNode* split_node(TreeNode *parent, Node *child) noexcept
    {
        assert(nullptr != parent && nullptr != child);

        typedef typename std::list<Node*>::const_iterator iter_type;

        // 收集所有的子节点
        std::list<Node*> remained;
        remained.push_back(child);
        for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
        {
            assert(nullptr != parent->child_at(i));
            remained.push_back(parent->child_at(i));
        }

        // 挑选两个种子，并分别作为 parent 和 uncle (parent的兄弟节点) 的一个子节点
        Node *seed1 = nullptr, *seed2 = nullptr;
        liner_pick_seeds(&remained, &seed1, &seed2);
        assert(nullptr != seed1 && nullptr != seed2);
        parent->clear_children();
        parent->area = seed1->area;
        parent->append_child(seed1);
        TreeNode *uncle = (TreeNode*) ::malloc(sizeof(TreeNode));
        assert(nullptr != uncle);
        new (uncle) TreeNode();
        uncle->append_child(seed2);
        uncle->fit_rect();

        size_t count1 = 1, count2 = 1;
        while (!remained.empty())
        {
            if (remained.size() == MIN_ENTRY_COUNT - count1)
            {
                for (iter_type iter = remained.begin(), end = remained.end();
                    iter != end; ++iter)
                {
                    parent->append_child(*iter);
                    parent->area.expand_to_contain((*iter)->area);
                }
                break;
            }
            else if (remained.size() == MIN_ENTRY_COUNT - count2)
            {
                for (iter_type iter = remained.begin(), end = remained.end();
                    iter != end; ++iter)
                {
                    uncle->append_child(*iter);
                    uncle->area.expand_to_contain((*iter)->area);
                }
                break;
            }

            Node *e = pick_next(&remained, parent->area, uncle->area);
            assert(nullptr != e);
            float_type el1 = acreage_needed(parent->area, e->area),
                el2 = acreage_needed(uncle->area, e->area);
            if (el1 < el2 || (el1 == el2 && count1 < count2))
            {
                parent->append_child(e);
                parent->area.expand_to_contain(e->area);
                ++count1;
            }
            else
            {
                uncle->append_child(e);
                uncle->area.expand_to_contain(e->area);
                ++count2;
            }
        }

        return uncle;
    }

    /**
     * 从一堆子节点中选取两个合适的作为种子
     */
    void liner_pick_seeds(std::list<Node*> *children, Node **pseed1, Node **pseed2) noexcept
    {
        assert(nullptr != children && children->size() >= 2 &&
               nullptr != pseed1 && nullptr != pseed2);

        typedef std::list<Node*> list_type;
        typedef typename list_type::iterator iter_type;

        // 下面两组变量用来求各个维度中 children 占用的 range
        iter_type highest_high_side[DIMENSIONS];
        iter_type lowest_low_side[DIMENSIONS];

        // 下面两组变量用来求 children 在各个维度中分的最开的两个 child
        iter_type highest_low_side[DIMENSIONS];
        iter_type lowest_high_side[DIMENSIONS];

        // 初始化
        const iter_type end = children->end();
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            highest_high_side[i] = end;
            lowest_low_side[i] = end;
            highest_low_side[i] = end;
            lowest_high_side[i] = end;
        }

        // 给上述四组变量取值
        for (iter_type iter = children->begin(); iter != end; ++iter)
        {
            assert(nullptr != *iter);
            for (size_t i = 0; i < DIMENSIONS; ++i)
            {
                const area_type& area = (*iter)->area;

                if (highest_high_side[i] == end || area.higher[i] > (*(highest_high_side[i]))->area.higher[i])
                    highest_high_side[i] = iter;

                if (lowest_low_side[i] == end || area.lower[i] < (*(lowest_low_side[i]))->area.lower[i])
                    lowest_low_side[i] = iter;

                // 这两组变量在各个维度中不能取相同的值
                if (highest_low_side[i] == end)
                    highest_low_side[i] = iter;
                else if (lowest_high_side[i] == end)
                    lowest_high_side[i] = iter;
                else if (area.lower[i] > (*(highest_low_side[i]))->area.lower[i])
                    highest_low_side[i] = iter;
                else if (area.higher[i] < (*(lowest_high_side[i]))->area.higher[i])
                    lowest_high_side[i] = iter;
            }
        }

        // 对比各个维度的分离度，取分离度最大的维度对应的一组数据
        int greatest_separation_idx = 0;
        float_type greatest_separation = 0;
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            float_type width = (*(highest_high_side[i]))->area.higher[i] -
                (*(lowest_low_side[i]))->area.lower[i];
            assert(width > 0);
            float_type separation = (*(highest_low_side[i]))->area.lower[i] -
                (*(lowest_high_side[i]))->area.higher[i];
            if (separation < 0)
                separation = -separation;
            float_type nomalize = separation / width;
            if (i == 0 || nomalize > greatest_separation)
            {
                greatest_separation_idx = (int) i;
                greatest_separation = nomalize;
            }
        }
        assert(highest_low_side[greatest_separation_idx] != lowest_high_side[greatest_separation_idx]);

        // 构造返回值，并从列表中删除选中的项
        *pseed1 = *(highest_low_side[greatest_separation_idx]);
        *pseed2 = *(lowest_high_side[greatest_separation_idx]);
        children->erase(highest_low_side[greatest_separation_idx]);
        children->erase(lowest_high_side[greatest_separation_idx]); // 由于是list，上次删除操作后迭代器还未失效
    }

    /**
     * 从剩余的零散节点中找到下一个适合添加到树上的节点
     */
    Node* pick_next(std::list<Node*> *remained, const area_type& r1, const area_type& r2) noexcept
    {
        assert(nullptr != remained);

        typedef typename std::list<Node*>::iterator iter_type;

        iter_type max_diff_index = remained->begin();
        float_type max_diff = 0;
        for (iter_type iter = remained->begin(), end = remained->end();
            iter != end; ++iter)
        {
            float_type diff = acreage_needed(r1, (*iter)->area) - acreage_needed(r2, (*iter)->area);
            if (diff < 0)
                diff = -diff;
            if (diff > max_diff)
            {
                max_diff_index = iter;
                max_diff = diff;
            }
        }
        Node *ret = *max_diff_index;
        assert(nullptr != ret);
        remained->erase(max_diff_index);
        return ret;
    }

    /**
     * 添加节点后调整 rtree
     *
     * @param n The parent node which has appended a new child
     * @param nn 可以为 nullptr
     */
    TreeNode* adjust_tree(TreeNode *n, TreeNode *nn) noexcept
    {
        assert(nullptr != n);
        while (true)
        {
            // adjust range of N
            n->fit_rect();

            // if N is root, stop
            TreeNode *parent = dynamic_cast<TreeNode*>(n->parent);
            if (nullptr == parent)
                return nn;

            if (nullptr != nn)
                nn = parent->append_child(nn) ? nullptr : split_node(parent, nn);
            n = parent;
        }
    }

    /**
     * 找第一个与给定区域相等的数据节点
     */
    DataNode* find_first_data_node(const area_type& r) noexcept
    {
        std::stack<TreeNode*> st;
        st.push(_root);
        while (!st.empty())
        {
            TreeNode *n = st.top();
            st.pop();
            assert(nullptr != n);

            for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
            {
                Node *child = n->child_at(i);
                if (nullptr == child)
                    break;
                if (child->area.contains(r))
                {
                    if (child->is_tree_node())
                        st.push(dynamic_cast<TreeNode*>(child));
                    else if (child->area == r)
                        return dynamic_cast<DataNode*>(child);
                }
            }
        }
        return nullptr;
    }

    /**
     * 找与给定区域相等且数据也相等的数据节点
     */
    DataNode* find_data_node(const area_type& r, const data_type& d) noexcept
    {
        std::stack<TreeNode*> st;
        st.push(_root);
        while (!st.empty())
        {
            TreeNode *n = st.top();
            st.pop();
            assert(nullptr != n);

            for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
            {
                Node *child = n->child_at(i);
                if (nullptr == child)
                    break;
                if (child->area.contains(r))
                {
                    if (child->is_tree_node())
                    {
                        st.push(dynamic_cast<TreeNode*>(child));
                    }
                    else if (child->area == r)
                    {
                        DataNode *dn = dynamic_cast<DataNode*>(child);
                        if (dn->data == d)
                            return dn;
                    }
                }
            }
        }
        return nullptr;
    }

    /**
     * 删除节点后调整rtree
     *
     * @param l The node whose child has been deleted
     */
    void condense_tree(TreeNode *l) noexcept
    {
        assert(nullptr != l);

        std::stack<TreeNode*> q;
        std::stack<size_t> qd;
        TreeNode *n = l;
        size_t depth = _height;
        while (depth > 1)
        {
            TreeNode* parent = n->parent;
            if (n->child_count() < MIN_ENTRY_COUNT)
            {
                n->parent->remove_child(n);
                q.push(n);
                qd.push(depth);
            }
            else
            {
                n->fit_rect();
            }
            n = parent;
            --depth;
        }
        while (!q.empty())
        {
            n = q.top();
            q.pop();
            assert(nullptr != n);

            depth = qd.top();
            qd.pop();

            for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
            {
                Node *e = n->child_at(i);
                if (nullptr == e)
                    break;
                insert(e, depth);
            }

            // 释放内存
            n->~TreeNode();
            ::free(n);
        }
        assert(qd.empty());
    }

private:
    TreeNode *_root = nullptr; // 根节点
    size_t _height = 0; // 高度，TreeNode的层数
    size_t _size = 0; // 容量
};

}

#endif /* head file guarder */
