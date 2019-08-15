
#include <limits.h>
#include <random>
#include <iostream>
#include <nut/unittest/unittest.h>

#include <nut/container/tree/rbtree.h>


using namespace nut;
using namespace std;


class TestRBTree : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_random);
        NUT_REGISTER_CASE(test_bug1);
        NUT_REGISTER_CASE(test_bug2);
        NUT_REGISTER_CASE(test_bug3);
        NUT_REGISTER_CASE(test_bug4);
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
    };

    void test_rand(size_t seed, size_t count)
    {
        mt19937_64 gen(seed);
        uniform_int_distribution<int> dist(0, INT_MAX);

        vector<Node*> nodes;
        Node *root = nullptr;
        for (size_t i = 0; i < count; ++i)
        {
            Node *n = new Node(dist(gen));
            root = RBTree<int,Node>::insert(root, n);
            nodes.push_back(n);
        }
        for (size_t i = 0; i < count; ++i)
        {
            Node *n = nodes.at(i);
            root = RBTree<int,Node>::remove(root, n);
            delete n;
        }
        NUT_TA(nullptr == root);
    }

    void test_random()
    {
        for (size_t i = 0; i < 1000; ++i)
        {
            // test_rand(i, 4);
            for (size_t j = 1; j < 50; ++j) // 调试时，长度可取短一些
                test_rand(i, j);
        }
    }

    void test_bug1()
    {
        Node *root = nullptr;
        Node *n = new Node(1);
        root = RBTree<int,Node>::insert(root, n);
        root = RBTree<int,Node>::remove(root, n); // bug: 这里会崩溃
        delete n;
    }

    void test_seq(int *data, size_t count)
    {
        Node *root = nullptr;
        vector<Node*> nodes;
        for (size_t i = 0; i < count; ++i)
        {
            Node *n = new Node(data[i]);
            root = RBTree<int,Node>::insert(root, n);
            nodes.push_back(n);
        }
        for (size_t i = 0; i < count; ++i)
        {
            Node *n = nodes.at(i);
            root = RBTree<int,Node>::remove(root, n);
            delete n;
        }
        NUT_TA(nullptr == root);
    }

    void test_bug2()
    {
        // bug: RBTree::remove() 函数中，使用了非标准的处理方式(交换 escaper 和
        //      to_be_del 节点的位置，而不是数据)，但是在处理指针时忘记修改 child
        //      中指向 to_be_del 的指针为指向 escaper，从而导致数据结构错乱
        int data[] = {2, 1, 3};
        test_seq(data, 3);
    }

    void test_bug3()
    {
        // bug: RBTree::remove() 函数中，使用了非标准的处理方式(交换 escaper 和
        //      to_be_del 节点的位置，而不是数据)，但是在交换完节点指针后，忘记
        //      更新 sublink_parent(其可能会指向 to_be_del, 但是我们交换了
        //      escaper 与 to_be_del 的位置，，囧)
        int data[] = {1, 3, 4, 2};
        test_seq(data, 4);
    }

    void test_bug4()
    {
        // bug: RBTree::_insert_fixup() 函数中，rotate 操作之后漏更新 'parent' 变量了
        int data[] = {2, 4, 3, 1};
        test_seq(data, 4);
    }
};

NUT_REGISTER_FIXTURE(TestRBTree, "container, tree, quiet")
