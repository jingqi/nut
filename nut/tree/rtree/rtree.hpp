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

#include <nut/debugging/static_assert.hpp>
#include <nut/util/tuple.hpp>

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
    /** 多维区域 */
    struct Rect
    {
        NumT left[DIMENSIONS];
        NumT right[DIMENSIONS];

        Rect()
        {
            for (register int i = 0; i < DIMENSIONS; ++i)
            {
                left[i] = 0;
                right[i] = 0;
            }
        }

        bool operator==(const Rect& x) const
        {
            for (register int i = 0; i < DIMENSIONS; ++i)
                if (left[i] != x.left[i] || right[i] != x.right[i])
                    return false
            return true;
        }

        bool operator!=(const Rect& x) const
        {
            return !(*this == x);
        }
        
        /** 所占的空间 */
        RealNumT acreage() const
        {
            RealNumT acr = 1;
            for (register int i = 0; i < DIMENSIONS; ++i)
            {
                acr *= right[i] - left[i];
            }
            return acr;
        }

        /** 扩展到包容指定的区域所需要扩展的空间 */
        RealNumT acreageNeeded(const Rect& x) const
        {
            RealNumT new_acr = 1;
            for (register int i = 0; i < DIMENSIONS; ++i)
            {
                new_acr *= max(right[i], x.right[i]) - min(left[i], x.left[i]);
            }
            return new_acr - acreage();
        }

        /** 扩展区域，以便包含目标区域 */
        void expandToContain(const Rect& x)
        {
            for (register int i = 0; i < DIMENSIONS; ++i)
            {
                left[i] = min(left[i], x.left[i]);
                right[i] = max(right[i], x.right[i]);
            }
        }

        bool contains(const Rect& x)
        {
            for (register int i = 0; i < DIMENSIONS; ++i)
                if (!(left[i] <= x.left[i] && x.right[i] <= right[i]))
                    return false;
            return true;
        }
    };

    /** 节点 */
    struct Node
    {
        Rect rect;
        Node *parent;
        bool treeNode;

        Node(bool tn) : parent(NULL), treeNode(treeNode) {}
        Node(const Rect& rt, bool tn) : rect(rt), parent(NULL), treeNode(tn) {}
    };

    /** 树节点 */
    struct TreeNode : public Node
    {
        Node *children[MAX_ENTRY_COUNT];

        TreeNode()
            : Node(true)
        {
            ::memset(children, 0, sizeof(Node*) * MAX_ENTRY_COUNT);
        }
        
        size_t childCount()
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
            return true;
        }

        bool removeChild(Node *child)
        {
            for (register int i = 0; i < MAX_ENTRY_COUNT && NULL != children[i]; ++i)
            {
                if (children[i] == child)
                {
                    child->parent = NULL;
                    children[i] = NULL;
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
                children[i] = NULL;
        }

        void fitRect()
        {
            assert(NULL != children[0]);
            rect = children[0]->rect;
            for (register int i = 1; i < MAX_ENTRY_COUNT && NULL != children[i]; ++i)
                rect.expandToContain(children[i]->rect);
        }
    };

    /** 数据节点 */
    struct DataNode : public Node
    {
        DataT data;

        DataNode(const DataT& v) : Node(false), data(v) {}
        DataNode(const Rect& rt, const DataT& v) : Node(rt, false), data(v) {}
    };

    typedef AllocT                                   data_allocator_type;
    typedef typename AllocT::rebind<TreeNode>::other treenode_allocator_type;
    typedef typename AllocT::rebind<DataNode>::other datanode_allocator_type;

    treenode_allocator_type m_treenodeAlloc;
    datanode_allocator_type m_datanodeAlloc;
    TreeNode *m_root;
    size_t m_height;
    size_t m_size;

public:
    RTree()
        : m_root(NULL), m_height(1), m_size(0)
    {
        NUT_STATIC_ASSERT(DIMENSIONS >= 2); // 空间维数
        NUT_STATIC_ASSERT(MAX_ENTRY_COUNT >= 2); // 最大子节点数
        NUT_STATIC_ASSERT(1 <= MIN_ENTRY_COUNT && MIN_ENTRY_COUNT <= (MAX_ENTRY_COUNT + 1) / 2); // 最小子节点数

        m_root = m_treenodeAlloc.allocate(1);
        new (m_root) TreeNode();
    }

    void insert(const Rect& rect, const DataT& data)
    {
        DataNode *dataNode = m_datanodeAlloc.allocate(1);
        assert(NULL != dataNode);
        new (dataNode) DataNode(rect, data);
        insert(dataNode, m_height);
        ++m_size;
    }

    bool removeFirst(const Rect& r)
    {
        DataNode *dn = findFirstDataNode(r);
        if (NULL == dn)
            return false;
        assert(NULL != dn->parent);
        TreeNode *l = dynamic_cast<TreeNode*>(dn->parent);

    }

    std::vector<DataT> search(const Rect& area)
    {}

private:
    void insert(Node *node, size_t depth)
    {
        assert(NULL != node);
        TreeNode *n = chooseNode(node->rect, depth);
        TreeNode *ll = n->appendChild(node) ? NULL : splitNode(n, node);
        TreeNode *r = adjustTree(n, ll);
        if (NULL != r)
        {
            // new root
            TreeNode *nln = m_treenodeAlloc.allocate(1);
            new (nln) TreeNode(m_root->rect, NULL);
            nln.appendChild(m_root);
            nln.appendChild(r);
            nln.expandToContain(r);
            m_root = nln;
            ++height;
        }
    }

    /** 根据目标区域选区适合的节点 */
    TreeNode* chooseNode(const Rect& rectToAdd, size_t depth)
    {
        TreeNode *ret = m_root;
        while (depth > 1)
        {
            TreeNode *nn = NULL;

            // choose the least enlargement child
            RealNumT least = 0;
            for (register int i = 0; i < MAX_ENTRY_COUNT && NULL != ret->children[i]; ++i)
            {
                RealNumT el = ret->children[i]->rect.acreageNeeded(rectToAdd);
                if (0 == i || el < least)
                {
                    nn = dynamic_cast<TreeNode*>(ret->children[i]);
                    least = el;
                }
            }
            ret = nn;
            --depth;
        }
        return ret;
    }
    
    /** 拆分节点 */
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
        parent->rect = seeds.first->rect;
        parent->appendChild(seeds.first);
        Node *uncle = m_treenodeAlloc.allocate(1);
        assert(NULL != uncle);
        new (uncle) TreeNode(seeds.second->rect, NULL);
        uncle->appendChild(seeds.second);

        int count1 = 1, count2 = 1;
        while (!remained.empty())
        {
            if (remained.size() == MIN_ENTRY_COUNT - count1)
            {
                for (std::list<Node*>::iterator iter = remained.begin(), end = remained.end();
                    iter != end; ++iter)
                {
                    parent->appendChild(*iter);
                    parent->rect.expandToContain((*iter)->rect);
                }
                break;
            }
            else if (remained.size() == MIN_ENTRY_COUNT - count2)
            {
                for (std::list<Node*>::iterator iter = remained.begin(), end = remained.end();
                    iter != end; ++iter)
                {
                    uncle->appendChild(*iter);
                    uncle->rect.expandToContain((*iter)->rect);
                }
                break;
            }

            TreeNode *e = pickNext(&remained, parent->rect, uncle->rect);
            assert(NULL != e);
            RealNumT el1 = parent->rect.acreageNeeded(e->rect), el2 = uncle->rect.acreageNeeded(e->rect);
            if (el1 < el2 || (el1 == el2 && count1 < count2))
            {
                parent->appendChild(e);
                parent->rect.expandToContain(e->rect);
                ++count1;
            }
            else
            {
                uncle->appendChild(e);
                uncle->rect.expandToContain(e->rect);
                ++count2;
            }
        }

        return uncle;
    }
    
    Tuple<Node*, Node*> linerPickSeeds(std::list<Node*> *children)
    {
        assert(NULL != children);
        typedef std::list<Node*> list_t;
        typedef list_t::iterator iter_t;
        iter_t highestLowSide[DIMENSIONS];
        iter_t lowestHighSide[DIMENSIONS];
        iter_t highestHighSide[DIMENSIONS];
        iter_t lowestLowSide[DIMENSIONS];
        for (iter_t iter = children->begin(), end = children->end();
            iter != end; ++iter)
        {
            assert(NULL != *iter);
            for (register int j = 0; j < DIMENSIONS; ++j)
            {
                if (i == 0 || (*iter)->rect.left[j] < (*lowestLowSide)[j]->rect.left[j])
                    lowestLowSide[j] = iter;
                if (i == 0 || (*iter)->rect.left[j] > (*highestLowSide)[j]->rect.left[j])
                    highestLowSide[j] = iter;
                if (i == 0 || (*iter)->rect.right[j] < (*lowestHighSide)[j]->rect.right[j])
                    lowestHighSide[j] = iter;
                if (i == 0 || (*iter)->rect.right[j] > (*highestHighSide)[j]->rect.right[j])
                    highestHighSide[j] = iter;
            }
        }

        int greatest_separation_idx = 0;
        RealNumT greatest_separation = 0;
        for (register int i = 0; i < DIMENSIONS; ++i)
        {
            RealNumT width = highestHighSide[i]->rect.right[i] - lowestLowSide[i]->rect.left[i];
            RealNumT separation = highestLowSide[i]->rect.left[i] - lowestHighSide[i]->rect.right[i];
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
        Tuple<Node*, Node*> ret(*highestLowSide[greatest_separation_idx], *lowestHighSide[greatest_separation_idx]);
        children->erase(highestLowSide[greatest_separation_idx]);
        children->erase(lowestHighSide[greatest_separation_idx]); // 由于是list，上次删除操作后迭代器还未失效
        return ret;
    }

    Node* pickNext(std::list<Node*> *remained, const Rect& r1, const Rect& r2)
    {
        assert(NULL != remained);
        std::list<Node*>::iterator maxDiffIndex = remained->begin();
        RealNumT maxDiff = 0;
        for (std::list<Node*>::iterator iter = remained->begin(), end = iter->end();
            iter != end; ++iter)
        {
            RealNumT diff = r1.acreageNeeded((*iter)->rect) - r2.acreageNeeded((*iter)->rect);
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

    TreeNode* adjustTree(TreeNode *n, TreeNode *nn)
    {
        assert(NULL != n && NULL != nn);
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

    DataNode* findFirstDataNode(const Rect& r)
    {
        std::stack<TreeNode*> st;
        st.push(m_root);
        while (!st.empty())
        {
            TreeNode *n = st.pop();
            assert(NULL != n);
            for (register int i = 0; i < MAX_ENTRY_COUNT && NULL != n->children[i]; ++i)
            {
                Node *child = n->children[i];
                if (child->rect.contains(r))
                {
                    if (child->treeNode)
                        st.push(dynamic_cast<TreeNode*>(child));
                    else
                        return dynamic_cast<DataNode*>(child);
                }
            }
        }
        return NULL;
    }
};

}

#endif /* head file guarder */


