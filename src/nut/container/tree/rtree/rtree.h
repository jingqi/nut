﻿
#ifndef ___HEADFILE_160547E9_5A30_4A78_A5FF_76E0C5EBE229_
#define ___HEADFILE_160547E9_5A30_4A78_A5FF_76E0C5EBE229_

#include <list>
#include <vector>
#include <stack>
#include <map> // for pair
#include <assert.h>

#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_CC_VC)
#   include <allocators>
#   include <xutility>
#endif

#include <nut/container/tuple.h>

#include "mdarea.h"

namespace nut
{

/**
 * 多维rtree
 *
 * 参考资料：
 *      R-Trees - A Dynamic index structure for spatial searching. Antonin Guttman. University of California Berkeley
 *
 * @param DataT 数据类型; 可以是 int、void*, obj* 等
 * @param NumT 数字类型; 可以是 int、float 等
 * @param DIMENSIONS 区域维数; 大于等于2
 * @param RealNumT 实数，避免乘法运算溢出，常用的是float，double等
 * @param MAX_ENTRY_COUNT 节点最大孩子数; 大于等于2
 * @param MIN_ENTRY_COUNT 节点最小孩子数; 小于最大孩子数
 * @param AllocT 内存分配器
 */
template <typename DataT, typename NumT, size_t DIMENSIONS = 2, typename RealNumT = double,
    size_t MAX_ENTRY_COUNT = 5, size_t MIN_ENTRY_COUNT = MAX_ENTRY_COUNT / 2,
    typename AllocT = std::allocator<DataT> >
class RTree
{
    static_assert(DIMENSIONS >= 2, "rtree 空间维数");
    static_assert(MAX_ENTRY_COUNT >= 2, "rtree 最大子节点数");
    static_assert(1 <= MIN_ENTRY_COUNT && MIN_ENTRY_COUNT <= (MAX_ENTRY_COUNT + 1) / 2, "rtree 最小子节点数");

public:
    typedef MDArea<NumT, DIMENSIONS, RealNumT>  area_type;
    typedef DataT                               data_type;

private:
    /**
     * 节点基类
     */
    struct TreeNode;
    struct Node
    {
        area_type area;
        TreeNode *parent = NULL;
        const bool tree_node = true; // 是树节点还是数据节点

        Node(bool tn)
            : tree_node(tn)
        {}

        Node(const area_type& rt, bool tn)
            : area(rt), tree_node(tn)
        {}

        virtual ~Node()
        {}

        bool is_tree_node() const
		{
			return tree_node;
		}
    };

    /**
     * 树节点
     */
    struct TreeNode : public Node
    {
        using Node::area;
        Node *children[MAX_ENTRY_COUNT];

        TreeNode()
            : Node(true)
        {
            ::memset(children, 0, sizeof(Node*) * MAX_ENTRY_COUNT);
        }

        Node* child_at(size_t i)
        {
            assert(i < MAX_ENTRY_COUNT);
            return children[i];
        }

        size_t child_count() const
        {
            // 二分查找
            int left = -1, right = MAX_ENTRY_COUNT;
            while (left + 1 < right)
            {
                int middle = (left + right) / 2;
                if (NULL == children[middle])
                    right = middle;
                else
                    left = middle;
            }
            return (size_t) right;
        }

        bool append_child(Node *child, bool set_child_parent = true)
        {
            assert(NULL != child);

            if (NULL != children[MAX_ENTRY_COUNT - 1])
                return false;
            children[child_count()] = child;
            if (set_child_parent)
                child->parent = this; // 附带设置 parent
            return true;
        }

        bool remove_child(Node *child, bool set_child_parent = true)
        {
            assert(NULL != child);
            for (size_t i = 0; i < MAX_ENTRY_COUNT && NULL != children[i]; ++i)
            {
                if (children[i] == child)
                {
                    if (set_child_parent)
                        child->parent = NULL; // 附带设置 parent
                    children[i] = NULL;

                    // 保持紧凑
                    for (size_t j = i; j + 1 < MAX_ENTRY_COUNT && NULL != children[j + 1]; ++j)
                    {
                        children[j] = children[j + 1];
                        children[j + 1] = NULL;
                    }
                    return true;
                }
            }
            return false;
        }

        void clear_children(bool set_child_parent = true)
        {
            for (size_t i = 0; i < MAX_ENTRY_COUNT && NULL != children[i]; ++i)
            {
                if (set_child_parent)
                    children[i]->parent = NULL; // 附带设置 parent
                children[i] = NULL;
            }
        }

        /**
         * 调整区域，使父节点区域恰好包含所有子节点区域
         */
        void fit_rect()
        {
            assert(NULL != children[0]);
            area = children[0]->area;
            for (size_t i = 1; i < MAX_ENTRY_COUNT && NULL != children[i]; ++i)
                area.expand_to_contain(children[i]->area);
        }
    };

    /**
     * 数据节点
     */
    struct DataNode : public Node
    {
        data_type data;

        DataNode(const data_type& v)
            : Node(false), data(v)
        {}

        DataNode(const area_type& rt, const data_type& v)
            : Node(rt, false), data(v)
        {}
    };

private:
    typedef RTree<DataT, NumT, DIMENSIONS, RealNumT, MAX_ENTRY_COUNT, MIN_ENTRY_COUNT, AllocT>  self;
    typedef AllocT                                                                              data_allocator_type;
    typedef typename AllocT::template rebind<TreeNode>::other                                   treenode_allocator_type;
    typedef typename AllocT::template rebind<DataNode>::other                                   datanode_allocator_type;

    treenode_allocator_type _tree_node_alloc;
    datanode_allocator_type _data_node_alloc;
    TreeNode *_root = NULL; // 根节点
    size_t _height = 0; // 高度，TreeNode的层数
    size_t _size = 0; // 容量

    /**
     * 扩展到包容指定的区域所需要扩展的空间
     */
    static RealNumT acreage_needed(const area_type& x, const area_type& y)
    {
        RealNumT new_acr = 1;
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            new_acr *= std::max(x.higher[i], y.higher[i]) - std::min(x.lower[i], y.lower[i]);
        }
        return new_acr - x.acreage();
    }

public:
    RTree()
    {
        _root = _tree_node_alloc.allocate(1);
        assert(NULL != _root);
        new (_root) TreeNode();
        _height = 1;
    }

    ~RTree()
    {
        clear();
        assert(NULL != _root);
        _root->~TreeNode();
        _tree_node_alloc.deallocate(_root, 1);
        _root = NULL;
    }

    /**
     * 插入数据
     */
    void insert(const area_type& rect, const data_type& data)
    {
        DataNode *data_node = _data_node_alloc.allocate(1);
        assert(NULL != data_node);
        new (data_node) DataNode(rect, data);
        insert(data_node, _height);
        ++_size;
    }

    /**
     * 移除第一个与给定区域相同的数据
     */
    bool remove_first(const area_type& rect)
    {
        // 找到数据节点并删除数据
        DataNode *dn = find_first_data_node(rect);
        if (NULL == dn)
            return false;
        assert(NULL != dn->parent);
        TreeNode *l = dynamic_cast<TreeNode*>(dn->parent);
        assert(NULL != l);
        bool rs = l->remove_child(dn);
        if (!rs)
            return false;

        // 释放内存
        dn->~DataNode();
        _data_node_alloc.deallocate(dn, 1);

        // 调整树
        condense_tree(l);
        if (_root->child_count() == 1 && _root->child_at(0)->is_tree_node())
        {
            TreeNode *tobedel = _root;
            _root = dynamic_cast<TreeNode*>(_root->child_at(0));
            _root->parent = NULL;
            --_height;

            // 释放内存
            tobedel->~TreeNode();
            _tree_node_alloc.deallocate(tobedel, 1);
        }
        --_size;
        return true;
    }

    /**
     * 移除指定的映射
     */
    bool remove(const area_type& rect, const data_type& data)
    {
        // 找到数据节点并删除数据
        DataNode *dn = find_data_node(rect, data);
        if (NULL == dn)
            return false;
        assert(NULL != dn->parent);
        TreeNode *l = dynamic_cast<TreeNode*>(dn->parent);
        assert(NULL != l);
        bool rs = l->remove_child(dn);
        if (!rs)
            return false;

        // 释放内存
        dn->~DataNode();
        _data_node_alloc.deallocate(dn, 1);

        // 调整树
        condense_tree(l);
        if (_root->child_count() == 1 && _root->child_at(0)->is_tree_node())
        {
            TreeNode *tobedel = _root;
            _root = dynamic_cast<TreeNode*>(_root->child_at(0));
            _root->parent = NULL;
            --_height;

            // 释放内存
            tobedel->~TreeNode();
            _tree_node_alloc.deallocate(tobedel, 1);
        }
        --_size;
        return true;
    }

    /**
     * 清除所有数据
     */
    void clear()
    {
        // 清除所有
        std::stack<TreeNode*> s;
        s.push(_root);
        while (!s.empty())
        {
            TreeNode *n = s.top();
            s.pop();

            for (size_t i = 0; i < MAX_ENTRY_COUNT && n->children[i] != NULL; ++i)
            {
                Node *c = n->child_at(i);
                if (c->is_tree_node())
                {
                    TreeNode *tn = dynamic_cast<TreeNode*>(c);
                    s.push(tn);
                }
                else
                {
                    DataNode *dn = dynamic_cast<DataNode*>(c);
                    dn->~DataNode();
                    _data_node_alloc.deallocate(dn, 1);
                }
            }
            if (n != _root)
            {
                n->~TreeNode();
                _tree_node_alloc.deallocate(n, 1);
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
    void search_intersect(const area_type& rect, std::vector<std::pair<area_type,data_type> > *appended)
    {
        assert(NULL != appended);

        std::stack<TreeNode*> s;
        s.push(_root);
        while (!s.empty())
        {
            TreeNode *n = s.top();
            s.pop();

            for (size_t i = 0; i < MAX_ENTRY_COUNT && n->children[i] != NULL; ++i)
            {
                Node *c = n->child_at(i);
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
                    appended->push_back(std::pair<area_type,data_type>(dn->area, dn->data));
                }
            }
        }
    }

    /**
     * 查找包含在指定区域内的数据
     */
    void search_contains(const area_type& rect, std::vector<std::pair<area_type, data_type> > *appended)
    {
        assert(NULL != appended);

        std::stack<TreeNode*> s;
        s.push(_root);
        while (!s.empty())
        {
            TreeNode *n = s.top();
            s.pop();

            for (size_t i = 0; i < MAX_ENTRY_COUNT && n->children[i] != NULL; ++i)
            {
                Node *c = n->child_at(i);
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
    void get_all(std::vector<data_type> *appended)
    {
        assert(NULL != appended);

        std::stack<TreeNode*> s;
        s.push(_root);
        while (!s.empty())
        {
            TreeNode *n = s.top();
            s.pop();

            for (size_t i = 0; i < MAX_ENTRY_COUNT && n->children[i] != NULL; ++i)
            {
                Node *c = n->child_at(i);
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

private:
    /**
     * 将节点插入深度为 depth 的位置
     */
    void insert(Node *node, size_t depth)
    {
        assert(NULL != node);

        TreeNode *n = choose_node(node->area, depth);
        TreeNode *ll = n->append_child(node) ? NULL : split_node(n, node);
        TreeNode *r = adjust_tree(n, ll);
        if (NULL != r)
        {
            // new root
            TreeNode *nln = _tree_node_alloc.allocate(1);
            assert(NULL != nln);
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
    TreeNode* choose_node(const area_type& rect_to_add, size_t depth)
    {
        TreeNode *ret = _root;
        while (depth > 1)
        {
            TreeNode *nn = NULL;

            // choose the least enlargement child
            RealNumT least = 0;
            for (size_t i = 0; i < MAX_ENTRY_COUNT && NULL != ret->children[i]; ++i)
            {
                RealNumT el = acreage_needed(ret->children[i]->area, rect_to_add);
                if (0 == i || el < least)
                {
                    nn = dynamic_cast<TreeNode*>(ret->children[i]); // 因为 depth > 1，这里应当是成功的
                    assert(NULL != nn);
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
    TreeNode* split_node(TreeNode *parent, Node *child)
    {
        assert(NULL != parent && NULL != child);

        typedef typename std::list<Node*>::const_iterator iter_t;

        // 收集所有的子节点
        std::list<Node*> remained;
        remained.push_back(child);
        for (size_t i = 0; i < MAX_ENTRY_COUNT; ++i)
        {
            assert(NULL != parent->children[i]);
            remained.push_back(parent->children[i]);
        }

        // 挑选两个种子，并分别作为 parent 和 uncle (parent的兄弟节点) 的一个子节点
        Node *seed1 = NULL, *seed2 = NULL;
        liner_pick_seeds(&remained, &seed1, &seed2);
        assert(NULL != seed1 && NULL != seed2);
        parent->clear_children();
        parent->area = seed1->area;
        parent->append_child(seed1);
        TreeNode *uncle = _tree_node_alloc.allocate(1);
        assert(NULL != uncle);
        new (uncle) TreeNode();
        uncle->append_child(seed2);
        uncle->fit_rect();

        size_t count1 = 1, count2 = 1;
        while (!remained.empty())
        {
            if (remained.size() == MIN_ENTRY_COUNT - count1)
            {
                for (iter_t iter = remained.begin(), end = remained.end();
                    iter != end; ++iter)
                {
                    parent->append_child(*iter);
                    parent->area.expand_to_contain((*iter)->area);
                }
                break;
            }
            else if (remained.size() == MIN_ENTRY_COUNT - count2)
            {
                for (iter_t iter = remained.begin(), end = remained.end();
                    iter != end; ++iter)
                {
                    uncle->append_child(*iter);
                    uncle->area.expand_to_contain((*iter)->area);
                }
                break;
            }

            Node *e = pick_next(&remained, parent->area, uncle->area);
            assert(NULL != e);
            RealNumT el1 = acreage_needed(parent->area, e->area), el2 = acreage_needed(uncle->area, e->area);
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
    void liner_pick_seeds(std::list<Node*> *children, Node **pseed1, Node **pseed2)
    {
        assert(NULL != children && children->size() >= 2 && NULL != pseed1 && NULL != pseed2);

        typedef std::list<Node*> list_t;
        typedef typename list_t::iterator iter_t;

        // 下面两组变量用来求各个维度中 children 占用的 range
        iter_t highest_high_side[DIMENSIONS];
        iter_t lowest_low_side[DIMENSIONS];

        // 下面两组变量用来求 children 在各个维度中分的最开的两个 child
        iter_t highest_low_side[DIMENSIONS];
        iter_t lowest_high_side[DIMENSIONS];

        // 初始化
        const iter_t end = children->end();
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            highest_high_side[i] = end;
            lowest_low_side[i] = end;
            highest_low_side[i] = end;
            lowest_high_side[i] = end;
        }

        // 给上述四组变量取值
        for (iter_t iter = children->begin(); iter != end; ++iter)
        {
            assert(NULL != *iter);
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
        RealNumT greatest_separation = 0;
        for (size_t i = 0; i < DIMENSIONS; ++i)
        {
            RealNumT width = (*(highest_high_side[i]))->area.higher[i] - (*(lowest_low_side[i]))->area.lower[i];
            assert(width > 0);
            RealNumT separation = (*(highest_low_side[i]))->area.lower[i] - (*(lowest_high_side[i]))->area.higher[i];
            if (separation < 0)
                separation = -separation;
            RealNumT nomalize = separation / width;
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
    Node* pick_next(std::list<Node*> *remained, const area_type& r1, const area_type& r2)
    {
        assert(NULL != remained);

        typedef typename std::list<Node*>::iterator iter_t;

        iter_t max_diff_index = remained->begin();
        RealNumT max_diff = 0;
        for (iter_t iter = remained->begin(), end = remained->end();
            iter != end; ++iter)
        {
            RealNumT diff = acreage_needed(r1, (*iter)->area) - acreage_needed(r2, (*iter)->area);
            if (diff < 0)
                diff = -diff;
            if (diff > max_diff)
            {
                max_diff_index = iter;
                max_diff = diff;
            }
        }
        Node *ret = *max_diff_index;
        assert(NULL != ret);
        remained->erase(max_diff_index);
        return ret;
    }

    /**
     * 添加节点后调整 rtree
     *
     * @param n The parent node which has appended a new child
     * @param nn 可以为 NULL
     */
    TreeNode* adjust_tree(TreeNode *n, TreeNode *nn)
    {
        assert(NULL != n);
        while (true)
        {
            // adjust range of N
            n->fit_rect();

            // if N is root, stop
            TreeNode *parent = dynamic_cast<TreeNode*>(n->parent);
            if (NULL == parent)
                return nn;

            if (NULL != nn)
                nn = parent->append_child(nn) ? NULL : split_node(parent, nn);
            n = parent;
        }
    }

    /**
     * 找第一个与给定区域相等的数据节点
     */
    DataNode* find_first_data_node(const area_type& r)
    {
        std::stack<TreeNode*> st;
        st.push(_root);
        while (!st.empty())
        {
            TreeNode *n = st.top();
            st.pop();
            assert(NULL != n);

            for (size_t i = 0; i < MAX_ENTRY_COUNT && NULL != n->children[i]; ++i)
            {
                Node *child = n->children[i];
                if (child->area.contains(r))
                {
                    if (child->tree_node)
                        st.push(dynamic_cast<TreeNode*>(child));
                    else if (child->area == r)
                        return dynamic_cast<DataNode*>(child);
                }
            }
        }
        return NULL;
    }

    /**
     * 找与给定区域相等且数据也相等的数据节点
     */
    DataNode* find_data_node(const area_type& r, const data_type& d)
    {
        std::stack<TreeNode*> st;
        st.push(_root);
        while (!st.empty())
        {
            TreeNode *n = st.top();
            st.pop();
            assert(NULL != n);

            for (size_t i = 0; i < MAX_ENTRY_COUNT && NULL != n->children[i]; ++i)
            {
                Node *child = n->children[i];
                if (child->area.contains(r))
                {
                    if (child->tree_node)
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
        return NULL;
    }

    /**
     * 删除节点后调整rtree
     *
     * @param l The node whose child has been deleted
     */
    void condense_tree(TreeNode *l)
    {
        assert(NULL != l);

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
            assert(NULL != n);

            depth = qd.top();
            qd.pop();

            for (size_t i = 0; i < MAX_ENTRY_COUNT && n->children[i] != NULL; ++i)
                insert(n->children[i], depth);

            // 释放内存
            n->~TreeNode();
            _tree_node_alloc.deallocate(n, 1);
        }
        assert(qd.empty());
    }

public:
    /**
     * 元素个数
     */
    size_t size() const
    {
        return _size;
    }

    /**
     * 树高，大于等于1
     */
    size_t height() const
    {
        return _height;
    }

    /**
     * 检查 rtree 结构是否错误
     */
    bool is_valid(Node *e = NULL, size_t depth = 1)
    {
        if (NULL == e)
        {
            e = _root;
            depth = 1;
        }
        assert(NULL != e && 1 <= depth && depth <= _height + 1);

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
        for (size_t i = 0; i < MAX_ENTRY_COUNT && n->children[i] != NULL; ++i)
        {
            Node *ee = n->child_at(i);
            if (!n->area.contains(ee->area))
                return false; // area error
            if (!is_valid(ee, depth + 1))
                return false;
        }

        return true;
    }
};

}

#endif /* head file guarder */
