
#include <iostream>
#include <nut/unittest/unittest.h>

#include <nut/container/tree/binary_tree.h>
#include <nut/container/tree/bstree.h>


using namespace nut;

class TestBSTree : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
        NUT_REGISTER_CASE(test_inorder_iterator);
        NUT_REGISTER_CASE(test_preorder_iterator);
        NUT_REGISTER_CASE(test_postorder_iterator);
    }

    struct Node
    {
        int key;
        Node *parent = nullptr;
        Node *left = nullptr;
        Node *right = nullptr;

        Node(int k)
            : key(k)
        {}

        ~Node()
        {
            // XXX 由于使用了 BinaryTree::delete_tree()，其实不用递归调用子节点
            // 的删除，这里只是用来测试一下是否会重复删除
            if (nullptr != left)
                delete left;
            if (nullptr != right)
                delete right;
        }

        int get_key() const { return key; }
        Node* get_parent() const { return parent; }
        Node* get_left_child() const { return left; }
        Node* get_right_child() const { return right; }
        void set_parent(Node *p) { parent = p; }
        void set_left_child(Node *p) { left = p; }
        void set_right_child(Node *p) { right = p; }
    };

    Node *root;

    virtual void set_up() override
    {
        /*
         * 构建这样一颗树
         *       4
         *     /   \
         *    2     6
         *   / \   / \
         *  1   3 5   7
         */
        root = nullptr;
        int nodes[7] = {4, 2, 6, 1, 3, 5, 7 };
        for (int i = 0; i < 7; ++i)
            root = BSTree<int,Node>::insert(root, new Node(nodes[i]));
    }

    virtual void tear_down() override
    {
        BinaryTree<Node>::delete_tree(root, [] (Node *n) { delete n; });
        root = nullptr;
    }

    void test_smoking()
    {
    }

    void test_inorder_iterator()
    {
        // 中序遍历
        int i = 0;
        int inorder[7] = {1, 2, 3, 4, 5, 6, 7 };
        for (BinaryTree<Node>::inorder_iterator iter = BinaryTree<Node>::inorder_traversal_begin(root),
                 end = BinaryTree<Node>::inorder_traversal_end(root);
             iter != end; ++iter)
        {
            NUT_TA(iter->get_key() == inorder[i]);
            ++i;
        }
        NUT_TA(7 == i);

        for (BinaryTree<Node>::inorder_reverse_iterator iter = BinaryTree<Node>::inorder_traversal_rbegin(root),
                 end = BinaryTree<Node>::inorder_traversal_rend(root);
             iter != end; ++iter)
        {
            --i;
            NUT_TA(iter->get_key() == inorder[i]);
        }
        NUT_TA(0 == i);
    }

    void test_preorder_iterator()
    {
        // 前序遍历
        int order[7] = {4, 2, 1, 3, 6, 5, 7 };
        int i = 0;
        for (BinaryTree<Node>::preorder_iterator iter = BinaryTree<Node>::preorder_traversal_begin(root),
                 end = BinaryTree<Node>::preorder_traversal_end(root);
             iter != end; ++iter)
        {
            NUT_TA(iter->get_key() == order[i]);
            ++i;
        }
        NUT_TA(7 == i);

        for (BinaryTree<Node>::preorder_reverse_iterator iter = BinaryTree<Node>::preorder_traversal_rbegin(root),
                 end = BinaryTree<Node>::preorder_traversal_rend(root);
             iter != end; ++iter)
        {
            --i;
            NUT_TA(iter->get_key() == order[i]);
        }
        NUT_TA(0 == i);
    }

    void test_postorder_iterator()
    {
        // 后序遍历
        int order[7] = {1, 3, 2, 5, 7, 6, 4 };
        int i = 0;
        for (BinaryTree<Node>::postorder_iterator iter = BinaryTree<Node>::postorder_traversal_begin(root),
                 end = BinaryTree<Node>::postorder_traversal_end(root);
             iter != end; ++iter)
        {
            NUT_TA(iter->get_key() == order[i]);
            ++i;
        }
        NUT_TA(7 == i);

        for (BinaryTree<Node>::postorder_reverse_iterator iter = BinaryTree<Node>::postorder_traversal_rbegin(root),
                 end = BinaryTree<Node>::postorder_traversal_rend(root);
             iter != end; ++iter)
        {
            --i;
            NUT_TA(iter->get_key() == order[i]);
        }
        NUT_TA(0 == i);
    }
};

NUT_REGISTER_FIXTURE(TestBSTree, "container, tree, quiet")
