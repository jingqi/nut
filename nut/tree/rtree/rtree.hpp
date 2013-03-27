/**
 * @file -
 * @author jingqi
 * @date 2012-03-10
 * @last-edit 2012-03-11 15:04:54 jingqi
 */

#ifndef ___HEADFILE_160547E9_5A30_4A78_A5FF_76E0C5EBE229_
#define ___HEADFILE_160547E9_5A30_4A78_A5FF_76E0C5EBE229_

#include <list>
#include <vector>
#include <stack>
#include <allocators>
#include <xutility>

#include <nut/debugging/static_assert.hpp>
#include <nut/util/tuple.hpp>

#include "mdarea.hpp"

namespace nut
{

/**
 * 多维rtree
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
public:
    typedef MDArea<NumT, DIMENSIONS, RealNumT> Area;

private:
    /**
     * 节点基类
     */
    struct TreeNode;
    struct Node
    {
        Area area;
        TreeNode *parent;
        bool treeNode; // 是树节点还是数据节点

        Node(bool tn) : parent(NULL), treeNode(treeNode) {}
        Node(const Area& rt, bool tn) : area(rt), parent(NULL), treeNode(tn) {}
        virtual ~Node() {}

        inline bool isTreeNode() const { return treeNode; }
    };

    /**
     * 树节点
     */
    struct TreeNode : public Node
    {
        Node *children[MAX_ENTRY_COUNT];

        TreeNode()
            : Node(true)
        {
            ::memset(children, 0, sizeof(Node*) * MAX_ENTRY_COUNT);
        }

        inline Node* childAt(size_t i)
        {
            assert(i < MAX_ENTRY_COUNT);
            return children[i];
        }
        
        size_t childCount() const
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
        
        bool appendChild(Node *child)
        {
            assert(NULL != child);

            if (NULL != children[MAX_ENTRY_COUNT - 1])
                return false;
            children[childCount()] = child;
            child->parent = this; // 附带设置 parent
            return true;
        }

        bool removeChild(Node *child)
        {
            for (register int i = 0; i < MAX_ENTRY_COUNT && NULL != children[i]; ++i)
            {
                if (children[i] == child)
                {
                    children[i]->parent = NULL; // 附带设置 parent
                    children[i] = NULL;

                    // 保持紧凑
                    for (register int j = i; j + 1 < MAX_ENTRY_COUNT && NULL != children[j + 1]; ++j)
                    {
                        children[j] = children[j + 1];
                        children[j + 1] = NULL;
                    }
                    return true;
                }
            }
            return false;
        }

        void clearChildren()
        {
            for (register int i = 0; i < MAX_ENTRY_COUNT && NULL != children[i]; ++i)
            {
                children[i]->parent = NULL; // 附带设置 parent
                children[i] = NULL;
            }
        }

        /**
         * 调整区域，使父节点区域恰好包含所有子节点区域
         */
        void fitRect()
        {
            assert(NULL != children[0]);
            area = children[0]->area;
            for (register int i = 1; i < MAX_ENTRY_COUNT && NULL != children[i]; ++i)
                area.expandToContain(children[i]->area);
        }
    };

    /**
     * 数据节点
     */
    struct DataNode : public Node
    {
        DataT data;

        DataNode(const DataT& v) : Node(false), data(v) {}
        DataNode(const Area& rt, const DataT& v) : Node(rt, false), data(v) {}
    };

    typedef AllocT                                   data_allocator_type;
    typedef typename AllocT::rebind<TreeNode>::other treenode_allocator_type;
    typedef typename AllocT::rebind<DataNode>::other datanode_allocator_type;

    treenode_allocator_type m_treenodeAlloc;
    datanode_allocator_type m_datanodeAlloc;
    TreeNode *m_root; // 根节点
    size_t m_height; // 高度，TreeNode的层数
    size_t m_size; // 容量

    /**
     * 扩展到包容指定的区域所需要扩展的空间
     */
    static RealNumT acreageNeeded(const Area& x, const Area& y)
    {
        RealNumT new_acr = 1;
        for (register int i = 0; i < DIMENSIONS; ++i)
        {
            new_acr *= std::max(x.higher[i], y.higher[i]) - std::min(x.lower[i], y.lower[i]);
        }
        return new_acr - x.acreage();
    }

public:
    RTree()
        : m_root(NULL), m_height(1), m_size(0)
    {
        NUT_STATIC_ASSERT(DIMENSIONS >= 2); // 空间维数
        NUT_STATIC_ASSERT(MAX_ENTRY_COUNT >= 2); // 最大子节点数
        NUT_STATIC_ASSERT(1 <= MIN_ENTRY_COUNT && MIN_ENTRY_COUNT <= (MAX_ENTRY_COUNT + 1) / 2); // 最小子节点数

        m_root = m_treenodeAlloc.allocate(1);
        assert(NULL != m_root);
        new (m_root) TreeNode();
    }

    /**
     * 插入数据
     */
    void insert(const Area& rect, const DataT& data)
    {
        DataNode *dataNode = m_datanodeAlloc.allocate(1);
        assert(NULL != dataNode);
        new (dataNode) DataNode(rect, data);
        insert(dataNode, m_height);
        ++m_size;
    }

    /**
     * 移除第一个与给定区域相同的数据
     */
    bool removeFirst(const Area& rect)
    {
        // 找到数据节点并删除数据
        DataNode *dn = findFirstDataNode(rect);
        if (NULL == dn)
            return false;
        assert(NULL != dn->parent);
        TreeNode *l = dynamic_cast<TreeNode*>(dn->parent);
        assert(NULL != l);
        bool rs = l->removeChild(dn);
        if (!rs)
            return false;

        // 调整树
        condenseTree(l);
        if (m_root->childCount() == 1 && m_root->childAt(0)->isTreeNode())
        {
            TreeNode *tobedel = m_root;
            m_root = dynamic_cast<TreeNode*>(m_root->childAt(0));
            m_root->parent = NULL;
            --m_height;

            tobedel->~TreeNode();
            m_treenodeAlloc.deallocate(tobedel, 1);
        }
        --m_size;
        return true;
    }

    bool remove(const Area& rect, const DataT& data)
    {
        // 找到数据节点并删除数据
        DataNode *dn = findDataNode(rect, data);
        if (NULL == dn)
            return false;
        assert(NULL != dn->parent);
        TreeNode *l = dynamic_cast<TreeNode*>(dn->parent);
        assert(NULL != l);
        bool rs = l->removeChild(dn);
        if (!rs)
            return false;

        // 调整树
        condenseTree(l);
        if (m_root->childCount() == 1 && m_root->childAt(0)->isTreeNode())
        {
            TreeNode *tobedel = m_root;
            m_root = dynamic_cast<TreeNode*>(m_root->childAt(0));
            m_root->parent = NULL;
            --m_height;

            tobedel->~TreeNode();
            m_treenodeAlloc.deallocate(tobedel, 1);
        }
        --m_size;
        return true;
    }

    /**
     * 查找与指定区域相交的数据
     */
    std::vector<DataT> searchIntersect(const Area& rect)
    {
        std::vector<DataT> ret;

        std::stack<TreeNode*> s;
        s.push(m_root);
        while (!s.empty())
        {
            TreeNode *n = s.top();
            s.pop();

            for (register size_t i = 0; i < MAX_ENTRY_COUNT && n->children[i] != NULL; ++i)
            {
                Node *c = n->childAt(i);
                if (!c->area.intersects(rect))
                    continue;

                if (c->isTreeNode())
                {
                    TreeNode *tn = dynamic_cast<TreeNode*>(c);
                    s.push(tn);
                }
                else
                {
                    DataNode *dn = dynamic_cast<DataNode*>(c);
                    ret.push_back(dn->data);
                }
            }
            return ret;
        }
    }

    /**
     * 查找包含在指定区域内的数据
     */
    std::vector<DataT> searchContains(const Area& rect)
    {
        std::vector<DataT> ret;

        std::stack<TreeNode*> s;
        s.push(m_root);
        while (!s.empty())
        {
            TreeNode *n = s.top();
            s.pop();

            for (register size_t i = 0; i < MAX_ENTRY_COUNT && n->children[i] != NULL; ++i)
            {
                Node *c = n->childAt(i);
                if (!c->area.intersects(rect))
                    continue;

                if (c->isTreeNode())
                {
                    TreeNode *tn = dynamic_cast<TreeNode*>(c);
                    s.push(tn);
                }
                else if (rect.contains(c->area))
                {
                    DataNode *dn = dynamic_cast<DataNode*>(c);
                    ret.push_back(dn->data);
                }
            }
            return ret;
        }
    }

    /**
     * 返回所有的数据
     */
    std::vector<DataT> getAll()
    {
        std::vector<DataT> ret;

        std::stack<TreeNode*> s;
        s.push(m_root);
        while (!s.empty())
        {
            TreeNode *n = s.top();
            s.pop();

            for (register size_t i = 0; i < MAX_ENTRY_COUNT && n->children[i] != NULL; ++i)
            {
                Node *c = n->childAt(i);
                if (c->isTreeNode())
                {
                    TreeNode *tn = dynamic_cast<TreeNode*>(c);
                    s.push(tn);
                }
                else
                {
                    DataNode *dn = dynamic_cast<DataNode*>(c);
                    ret.push_back(dn->data);
                }
            }
            return ret;
        }
    }

private:
    /**
     * 将节点插入深度为 depth 的位置
     */
    void insert(Node *node, size_t depth)
    {
        assert(NULL != node);

        TreeNode *n = chooseNode(node->area, depth);
        TreeNode *ll = n->appendChild(node) ? NULL : splitNode(n, node);
        TreeNode *r = adjustTree(n, ll);
        if (NULL != r)
        {
            // new root
            TreeNode *nln = m_treenodeAlloc.allocate(1);
            assert(NULL != nln);
            new (nln) TreeNode();
            nln->appendChild(m_root);
            nln->appendChild(r);
            nln->fitRect();
            m_root = nln;
            ++m_height;
        }
    }

    /**
     * 根据目标区域选区适合的节点
     */
    TreeNode* chooseNode(const Area& rectToAdd, size_t depth)
    {
        TreeNode *ret = m_root;
        while (depth > 1)
        {
            TreeNode *nn = NULL;

            // choose the least enlargement child
            RealNumT least = 0;
            for (register int i = 0; i < MAX_ENTRY_COUNT && NULL != ret->children[i]; ++i)
            {
                RealNumT el = acreageNeeded(ret->children[i]->area, rectToAdd);
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
    TreeNode* splitNode(TreeNode *parent, Node *child)
    {
        assert(NULL != parent && NULL != child);

        // 手机所有的子节点
        std::list<Node*> remained;
        remained.push_back(child);
        for (register int i = 0; i < MAX_ENTRY_COUNT; ++i)
        {
            assert(NULL != parent->children[i]);
            remained.push_back(parent->children[i]);
        }
        
        // 挑选两个种子，并分别作为 parent 和 uncle (parent的兄弟节点) 的一个子节点
        Tuple<Node*, Node*> seeds = linerPickSeeds(&remained);
        parent->clearChildren();
        parent->area = seeds.first->area;
        parent->appendChild(seeds.first);
        TreeNode *uncle = m_treenodeAlloc.allocate(1);
        assert(NULL != uncle);
        new (uncle) TreeNode();
        uncle->appendChild(seeds.second);
        uncle->fitRect();

        int count1 = 1, count2 = 1;
        while (!remained.empty())
        {
            if (remained.size() == MIN_ENTRY_COUNT - count1)
            {
                for (std::list<Node*>::iterator iter = remained.begin(), end = remained.end();
                    iter != end; ++iter)
                {
                    parent->appendChild(*iter);
                    parent->area.expandToContain((*iter)->area);
                }
                break;
            }
            else if (remained.size() == MIN_ENTRY_COUNT - count2)
            {
                for (std::list<Node*>::iterator iter = remained.begin(), end = remained.end();
                    iter != end; ++iter)
                {
                    uncle->appendChild(*iter);
                    uncle->area.expandToContain((*iter)->area);
                }
                break;
            }

            Node *e = pickNext(&remained, parent->area, uncle->area);
            assert(NULL != e);
            RealNumT el1 = acreageNeeded(parent->area, e->area), el2 = acreageNeeded(uncle->area, e->area);
            if (el1 < el2 || (el1 == el2 && count1 < count2))
            {
                parent->appendChild(e);
                parent->area.expandToContain(e->area);
                ++count1;
            }
            else
            {
                uncle->appendChild(e);
                uncle->area.expandToContain(e->area);
                ++count2;
            }
        }

        return uncle;
    }
    
    /**
     * 从一堆子节点中选取两个合适的作为种子
     */
    Tuple<Node*, Node*> linerPickSeeds(std::list<Node*> *children)
    {
        assert(NULL != children && children->size() >= 2);

        typedef std::list<Node*> list_t;
        typedef list_t::const_iterator iter_t;

        // 下面两组变量用来求各个维度中 children 占用的 range
        iter_t highestHighSide[DIMENSIONS];
        iter_t lowestLowSide[DIMENSIONS];

        // 下面两组变量用来求 children 在各个维度中分的最开的两个 child
        iter_t highestLowSide[DIMENSIONS];
        iter_t lowestHighSide[DIMENSIONS];

        // 初始化
        const iter_t end = children->end();
        for (register size_t i = 0; i < DIMENSIONS; ++i)
        {
            highestHighSide[i] = end;
            lowestLowSide[i] = end;
            highestLowSide[i] = end;
            lowestHighSide[i] = end;
        }

        // 给上述四组变量取值
        for (iter_t iter = children->begin(); iter != end; ++iter)
        {
            assert(NULL != *iter);
            for (register int i = 0; i < DIMENSIONS; ++i)
            {
                const Area& area = (*iter)->area;

                if (highestHighSide[i] == end || area.higher[i] > (*(highestHighSide[i]))->area.higher[i])
                    highestHighSide[i] = iter;

                if (lowestLowSide[i] == end || area.lower[i] < (*(lowestLowSide[i]))->area.lower[i])
                    lowestLowSide[i] = iter;

                // 这两组变量在各个维度中不能取相同的值
                if (highestLowSide[i] == end)
                    highestLowSide[i] = iter;
                else if (lowestHighSide[i] == end)
                    lowestHighSide[i] = iter;
                else if (area.lower[i] > (*(highestLowSide[i]))->area.lower[i])
                    highestLowSide[i] = iter;
                else if (area.higher[i] < (*(lowestHighSide[i]))->area.higher[i])
                    lowestHighSide[i] = iter;
            }
        }

        // 对比各个维度的分离度，取分离度最大的维度对应的一组数据
        int greatest_separation_idx = 0;
        RealNumT greatest_separation = 0;
        for (register int i = 0; i < DIMENSIONS; ++i)
        {
            RealNumT width = (*(highestHighSide[i]))->area.higher[i] - (*(lowestLowSide[i]))->area.lower[i];
            assert(width > 0);
            RealNumT separation = (*(highestLowSide[i]))->area.lower[i] - (*(lowestHighSide[i]))->area.higher[i];
            if (separation < 0)
                separation = -separation;
            RealNumT nomalize = separation / width;
            if (i == 0 || nomalize > greatest_separation)
            {
                greatest_separation_idx = i;
                greatest_separation = nomalize;
            }
        }
        assert(highestLowSide[greatest_separation_idx] != lowestHighSide[greatest_separation_idx]);

        // 构造返回值，并从列表中删除选中的项
        Tuple<Node*, Node*> ret(*(highestLowSide[greatest_separation_idx]), *(lowestHighSide[greatest_separation_idx]));
        children->erase(highestLowSide[greatest_separation_idx]);
        children->erase(lowestHighSide[greatest_separation_idx]); // 由于是list，上次删除操作后迭代器还未失效
        return ret;
    }

    /**
     * 从剩余的零散节点中找到下一个适合添加到树上的节点
     */
    Node* pickNext(std::list<Node*> *remained, const Area& r1, const Area& r2)
    {
        assert(NULL != remained);
        std::list<Node*>::iterator maxDiffIndex = remained->begin();
        RealNumT maxDiff = 0;
        for (std::list<Node*>::iterator iter = remained->begin(), end = remained->end();
            iter != end; ++iter)
        {
            RealNumT diff = acreageNeeded(r1, (*iter)->area) - acreageNeeded(r2, (*iter)->area);
            if (diff < 0)
                diff = -diff;
            if (diff > maxDiff)
            {
                maxDiffIndex = iter;
                maxDiff = diff;
            }
        }
        Node *ret = *maxDiffIndex;
        assert(NULL != ret);
        remained->erase(maxDiffIndex);
        return ret;
    }

    /**
     * 添加节点后调整 rtree
     *
     * @param n The parent node which has appended a new child
     * @param nn 可以为 NULL
     */
    TreeNode* adjustTree(TreeNode *n, TreeNode *nn)
    {
        assert(NULL != n);
        while (true)
        {
            // adjust range of N
            n->fitRect();

            // if N is root, stop
            TreeNode *parent = dynamic_cast<TreeNode*>(n->parent);
            if (NULL == parent)
                return nn;

            if (NULL != nn)
                nn = parent->appendChild(nn) ? NULL : splitNode(parent, nn);
            n = parent;
        }
    }

    /**
     * 找第一个与给定区域相等的数据节点
     */
    DataNode* findFirstDataNode(const Area& r)
    {
        std::stack<TreeNode*> st;
        st.push(m_root);
        while (!st.empty())
        {
            TreeNode *n = st.top();
            st.pop();
            assert(NULL != n);

            for (register int i = 0; i < MAX_ENTRY_COUNT && NULL != n->children[i]; ++i)
            {
                Node *child = n->children[i];
                if (child->area.contains(r))
                {
                    if (child->treeNode)
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
    DataNode* findDataNode(const Area& r, const DataT& d)
    {
        std::stack<TreeNode*> st;
        st.push(m_root);
        while (!st.empty())
        {
            TreeNode *n = st.top();
            st.pop();
            assert(NULL != n);

            for (register int i = 0; i < MAX_ENTRY_COUNT && NULL != n->children[i]; ++i)
            {
                Node *child = n->children[i];
                if (child->area.contains(r))
                {
                    if (child->treeNode)
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
    void condenseTree(TreeNode *l)
    {
        assert(NULL != l);

        std::stack<TreeNode*> q;
        std::stack<size_t> qd;
        TreeNode *n = l;
        size_t depth = m_height;
        while (depth > 1)
        {
            TreeNode* parent = n->parent;
            if (n->childCount() < MIN_ENTRY_COUNT)
            {
                n->parent->removeChild(n);
                q.push(n);
                qd.push(depth);
            }
            else
            {
                n->fitRect();
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

            for (register size_t i = 0; i < MAX_ENTRY_COUNT && n->children[i] != NULL; ++i)
                insert(n->children[i], depth);
        }
    }

public:
    /**
     * 元素个数
     */
    inline size_t size() const
    {
        return m_size;
    }

    /**
     * 树高，大于等于1
     */
    inline size_t height() const
    {
        return m_height;
    }

    /**
     * 检查 rtree 结构是否错误
     */
    bool isValid(Node *e = NULL, size_t depth = 1)
    {
        if (NULL == e)
        {
            e = m_root;
            depth = 1;
        }
        assert(NULL != e && 1 <= depth && depth <= m_height + 1);

        if (depth == m_height + 1)
        {
            if (e->isTreeNode())
                return false; // wrong node type with depth
            return true;
        }

        if (!e->isTreeNode())
            return false; // wrong node type with depth

        TreeNode *n = dynamic_cast<TreeNode*>(e);
        const int cc = n->childCount();
        if (depth != 1 && cc < MIN_ENTRY_COUNT)
            return false; // under fill
        for (register size_t i = 0; i < MAX_ENTRY_COUNT && n->children[i] != NULL; ++i)
        {
            Node *ee = n->childAt(i);
            if (!n->area.contains(ee->area))
                return false; // area error
            if (!isValid(ee, depth + 1))
                return false;
        }

        return true;
    }
};

}

#endif /* head file guarder */


