
#include <iostream>
#include <nut/unittest/unittest.hpp>

#include <nut/container/tree/btree.hpp>
#include <nut/container/tree/bstree.hpp>

using namespace nut;


NUT_FIXTURE(TestBSTree)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASE(testInorderIterator)
    NUT_CASE(testPreorderIterator)
    NUT_CASE(testPostorderIterator)
    NUT_CASES_END()

    struct Node
    {
        int key;
        Node *parent;
        Node *left;
        Node *right;

        Node(int k)
            : key(k), parent(NULL), left(NULL), right(NULL)
        {}

        int getKey() const { return key; }
        Node* getParent() const { return parent; }
        Node* getLeftChild() const { return left; }
        Node* getRightChild() const { return right; }
        void setParent(Node *p) { parent = p; }
        void setLeftChild(Node *p) { left = p; }
        void setRightChild(Node *p) { right = p; }

        void destroy()
        {
            if (NULL != left)
                left->destroy();
            if (NULL != right)
                right->destroy();
            delete this;
        }
    };

    Node *root;
    
    void setUp()
    {
        // 构建这样一颗树
        //       4
        //     /   \
        //    2     6
        //   / \   / \
        //  1   3 5   7
        //
        root = NULL;
        int nodes[7] = {4, 2, 6, 1, 3, 5, 7 };
        for (int i = 0; i < 7; ++i)
            root = BSTree<int,Node>::insert(root, new Node(nodes[i]));
    }

    void tearDown()
    {
        root->destroy();
        root = NULL;
    }

    void testSmoking()
    {
    }

    void testInorderIterator()
    {
        // 中序遍历
        int i = 0;
        int inorder[7] = {1, 2, 3, 4, 5, 6, 7 };
        for (BTree<Node>::inorder_iterator iter = BTree<Node>::inorder_traversal_begin(root), end = BTree<Node>::inorder_traversal_end(root);
            iter != end; ++iter)
        {
            NUT_TA(iter->getKey() == inorder[i]);
            ++i;
        }
        NUT_TA(7 == i);

        for (BTree<Node>::inorder_reverse_iterator iter = BTree<Node>::inorder_traversal_rbegin(root), end = BTree<Node>::inorder_traversal_rend(root);
            iter != end; ++iter)
        {
            --i;
            NUT_TA(iter->getKey() == inorder[i]);
        }
        NUT_TA(0 == i);
    }

    void testPreorderIterator()
    {
        // 前序遍历
        int order[7] = {4, 2, 1, 3, 6, 5, 7 };
        int i = 0;
        for (BTree<Node>::preorder_iterator iter = BTree<Node>::preorder_traversal_begin(root), end = BTree<Node>::preorder_traversal_end(root);
            iter != end; ++iter)
        {
            NUT_TA(iter->getKey() == order[i]);
            ++i;
        }
        NUT_TA(7 == i);

        for (BTree<Node>::preorder_reverse_iterator iter = BTree<Node>::preorder_traversal_rbegin(root), end = BTree<Node>::preorder_traversal_rend(root);
            iter != end; ++iter)
        {
            --i;
            NUT_TA(iter->getKey() == order[i]);
        }
        NUT_TA(0 == i);
    }

    void testPostorderIterator()
    {
        // 后序遍历
        int order[7] = {1, 3, 2, 5, 7, 6, 4 };
        int i = 0;
        for (BTree<Node>::postorder_iterator iter = BTree<Node>::postorder_traversal_begin(root), end = BTree<Node>::postorder_traversal_end(root);
            iter != end; ++iter)
        {
            NUT_TA(iter->getKey() == order[i]);
            ++i;
        }
        NUT_TA(7 == i);

        for (BTree<Node>::postorder_reverse_iterator iter = BTree<Node>::postorder_traversal_rbegin(root), end = BTree<Node>::postorder_traversal_rend(root);
            iter != end; ++iter)
        {
            --i;
            NUT_TA(iter->getKey() == order[i]);
        }
        NUT_TA(0 == i);
    }
};

NUT_REGISTER_FIXTURE(TestBSTree, "tree,quiet")
