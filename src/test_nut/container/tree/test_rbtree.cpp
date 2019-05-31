

#include <iostream>
#include <nut/unittest/unittest.h>

#include <nut/container/tree/rbtree.h>


using namespace nut;
using namespace std;


class TestRBTree : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_bug1);
    }

    struct Node
    {
        int key;
        bool red = false;
        Node *parent = nullptr;
        Node *left = nullptr;
        Node *right = nullptr;

        Node(int k)
            : key(k)
        {}

        int get_key() const { return key; }
        bool is_red() const { return red; }
        Node* get_parent() const { return parent; }
        Node* get_left_child() const { return left; }
        Node* get_right_child() const { return right; }
        void set_red(bool r) { red = r; }
        void set_parent(Node *p) { parent = p; }
        void set_left_child(Node *p) { left = p; }
        void set_right_child(Node *p) { right = p; }

        void destroy()
        {
            if (nullptr != left)
                left->destroy();
            if (nullptr != right)
                right->destroy();
            delete this;
        }
    };

    void test_bug1()
    {
        Node *root = nullptr;
        Node *n = new Node(1);
        root = RBTree<int,Node>::insert(root, n);
        root = RBTree<int,Node>::remove(root, n); // 这里会崩溃
        delete n;
    }
};

NUT_REGISTER_FIXTURE(TestRBTree, "container, tree, quiet")
