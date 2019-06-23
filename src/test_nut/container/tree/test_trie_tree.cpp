
#include <iostream>
#include <string>

#include <nut/unittest/unittest.h>
#include <nut/container/tree/trie_tree.h>


using namespace std;
using namespace nut;

class TestTrieTree : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
        NUT_REGISTER_CASE(test_put);
        NUT_REGISTER_CASE(test_remove);
        NUT_REGISTER_CASE(test_remove_tree);
        NUT_REGISTER_CASE(test_get_descendants);
        NUT_REGISTER_CASE(test_get_ancestors);
        NUT_REGISTER_CASE(test_iterator);
    }

    TrieTree<char,std::string> trie;

    virtual void set_up() override
    {
        trie.put("abc", 3, "c");
        trie.put("ab", 2, "b");
        trie.put("abde", 4, "e");
        trie.put("af", 2, "f");
        /*
         *              a
         *             / \
         *           [b] [f]
         *           / \
         *         [c]  d
         *               \
         *               [e]
         */
    }

    virtual void tear_down() override
    {
        trie.clear();
    }

    void test_smoking()
    {
        NUT_TA(trie.size() == 4);
        NUT_TA(*trie.get("abde", 4) == "e");
        NUT_TA(*trie.get("abc", 3) == "c");
        NUT_TA(*trie.get("ab", 2) == "b");
        NUT_TA(*trie.get("af", 2) == "f");
        NUT_TA(trie.get("a", 1) == nullptr);
        NUT_TA(trie.get("abd", 3) == nullptr);
        NUT_TA(trie.get("afd", 3) == nullptr);
    }

    void test_put()
    {
        NUT_TA(trie.put("abcgh", 5, "h") == 1);
        NUT_TA(*trie.get("abcgh", 5) == "h");

        NUT_TA(trie.put("abcgh", 5, "h1") == -1);
        NUT_TA(*trie.get("abcgh", 5) == "h1");

        NUT_TA(trie.put("abcgh", 5, "h2", false) == 0);
        NUT_TA(*trie.get("abcgh", 5) == "h1");
    }

    void test_remove()
    {
        NUT_TA(trie.size() == 4);
        NUT_TA(*trie.get("ab", 2) == "b");
        NUT_TA(trie.remove("ab", 2));
        NUT_TA(trie.size() == 3);
        NUT_TA(trie.get("ab", 2) == nullptr);
        NUT_TA(*trie.get("abc", 3) == "c");

        NUT_TA(!trie.remove("abd", 3));
        NUT_TA(!trie.remove("afg", 3));
        NUT_TA(trie.size() == 3);
    }

    void test_remove_tree()
    {
        NUT_TA(trie.remove_tree("af", 2) == 1);
        NUT_TA(trie.size() == 3);

        NUT_TA(trie.remove_tree("ab", 2) == 3);
        NUT_TA(trie.size() == 0);
    }

    void test_get_descendants()
    {
        // ab abc abde af
        NUT_TA(trie.has_descendants("", 0));
        NUT_TA(trie.get_descendants("", 0) == vector<string>({"b", "c", "e", "f"}));

        // ab abc abde
        NUT_TA(trie.has_descendants("ab", 2));
        NUT_TA(trie.get_descendants("ab", 2) == vector<string>({"b", "c", "e"}));

        // abde
        NUT_TA(trie.has_descendants("abd", 3));
        NUT_TA(trie.get_descendants("abd", 3) == vector<string>({"e"}));

        NUT_TA(trie.has_descendants("abde", 4));
        NUT_TA(!trie.has_descendants("abdef", 5));
    }

    void test_get_ancestors()
    {
        // ab
        NUT_TA(trie.has_ancestors("abd", 3));
        NUT_TA(trie.get_ancestors("abd", 3) == vector<string>({"b"}));

        // abde ab
        NUT_TA(trie.has_ancestors("abde", 4));
        NUT_TA(trie.get_ancestors("abde", 4) == vector<string>({"e", "b"}));
        NUT_TA(trie.has_ancestors("abdegh", 6));
        NUT_TA(trie.get_ancestors("abdegh", 6) == vector<string>({"e", "b"}));

        // abc ab
        NUT_TA(trie.has_ancestors("abc", 3));
        NUT_TA(trie.get_ancestors("abc", 3) == vector<string>({"c", "b"}));

        NUT_TA(!trie.has_ancestors("mf", 2));
    }

    void test_iterator()
    {
        // 迭代整棵树
        const char *seq[] = {"b", "c", "e", "f"};
        size_t i = 0;
        for (auto iter = trie.begin(), end = trie.end(); iter != end; ++iter)
        {
            NUT_TA(*iter == seq[i++]);
        }
        NUT_TA(4 == i);

        // 迭代子树
        const char *seq2[] = {"b", "c", "e"};
        i = 0;
        for (auto iter = trie.begin("ab", 2), end = trie.end("ab", 2);
             iter != end; ++iter)
        {
            NUT_TA(*iter == seq2[i++]);
        }
        NUT_TA(3 == i);

        // 语法糖
        i = 0;
        for (const string& s : trie)
            NUT_TA(s == seq[i++]);
        NUT_TA(4 == i);
    }
};

NUT_REGISTER_FIXTURE(TestTrieTree, "container, tree, quiet")
