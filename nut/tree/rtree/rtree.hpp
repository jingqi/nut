/**
 * @file -
 * @author jingqi
 * @date 2012-03-10
 * @last-edit 2012-03-11 15:04:54 jingqi
 */

#ifndef ___HEADFILE_160547E9_5A30_4A78_A5FF_76E0C5EBE229_
#define ___HEADFILE_160547E9_5A30_4A78_A5FF_76E0C5EBE229_

#include <vector>
#include <allocators>

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
        
        RealNumT acreage() const
        {
            RealNumT acr = 1;
            for (register int i = 0; i < DIMENSIONS; ++i)
            {
                acr *= right[i] - left[i];
            }
            return acr;
        }
        
        RealNumT acreageNeeded(const Rect& x) const
        {
            RealNumT new_acr = 1;
            for (register int i = 0; i < DIMENSIONS; ++i)
            {
                new_acr *= max(right[i], x.right[i]) - min(left[i] - x.left[i]);
            }
            return new_acr - acreage();
        }
    };

    /** 节点 */
    struct Node
    {
        Rect rect;
        Node *parent;

        Node() : parent(NULL) {}
        Node(const Rect& rt) : rect(rt), parent(NULL) {}
    };

    /** 树节点 */
    struct TreeNode : public Node
    {
        Node *children[MAX_ENTRY_COUNT];

        TreeNode()
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
    };

    /** 数据节点 */
    struct DataNode : public Node
    {
        DataT data;

        DataNode(const DataT& v) : data(v) {}
        DataNode(const Rect& rt, const DataT& v) : Node(rt), data(v) {}
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
        static_assert(DIMENSIONS >= 2, "Dimensions for rtree is too small");
        static_assert(MAX_ENTRY_COUNT >= 2, "Maximum entry count for rtree node is too small");
        static_assert(MIN_ENTRY_COUNT >= 1 && MIN_ENTRY_COUNT <= (MAX_ENTRY_COUNT + 1) / 2, "Minimum entry count for rtree node is not proper");

        m_root = m_treenodeAlloc.allocate(1);
        new (m_root) ();
    }

    void insert(const Rect& rect, const DataT& data)
    {
        DataNode *dataNode = m_datanodeAlloc.allocate(1);
        new (dataNode) (rect, data);
        insert(dataNode, m_height);
        ++m_size;
    }

    bool remove()
    {}

    std::vector<T> search(const Area& area)
    {}

private:
    void insert(Node *node, size_t depth)
    {
        assert(NULL != node);
        TreeNode *n = chooseNode(node->rect, depth);
        TreeNode *ll = n.appendChild(node) ? NULL : splitNode(n, node);
    }

    /** 根据目标区域选区适合的节点 */
    TreeNode* chooseNode(const Rect& rectToAdd, size_t depth)
    {
        TreeNode *ret = root;
        while (depth > 1)
        {
            TreeNode *nn = NULL;
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
    
    TreeNode* splitNode(TreeNode *parent, Node *child)
    {
        assert(NULL != parent && NULL != child);
        std::vector<Node*> remained;
        remained.push_back(child);
        for (register int i = 0; i < MAX_ENTRY_COUNT; ++i)
        {
            assert(NULL != parent->children[i]);
            remained.push_back(parent->children[i]);
        }
        
        Tuple<Node*, Node*> seeds = linerPickSeeds(remained);
    }
    
    Tuple<Node*, Node*> linerPickSeeds(const std::vector<Node*> children)
    {
        
    }
};

}

#endif /* head file guarder */


