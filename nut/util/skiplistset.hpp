/**
 * @file -
 * @author jingqi
 * @date 2013-08-24
 * @last-edit 2013-08-24 14:07:07 jingqi
 * @brief
 */

#ifndef ___HEADFILE_60C4D68A_A1D8_4B2C_A488_40E9A9FFE426_
#define ___HEADFILE_60C4D68A_A1D8_4B2C_A488_40E9A9FFE426_

#include <stdlib.h>
#include <new.h>

namespace nut
{

template <typename T>
class SkipListSet
{
    // 最大 level 数
    enum { MAX_LEVEL = 16 };

    class Node
    {
        T m_key;
        Node **m_next;
        size_t m_level; // 0-based

        public Node(const T& k)
            : m_key(k), m_next(NULL), m_level(0)
        {}
    };

    size_t m_level; // 0-based
    Node **m_head;
    size_t m_size;

private:
    /**
     * 随机化 level
     *
     * @return 0-based
     */
    static size_t randomLevel()
    {
        size_t k = 0;
        while (k < MAX_LEVEL && 0 != (rand() & 0x01))
            ++k;
        return k;
    }

    /**
     * 查找节点，附带查找各层前向节点
     *
     * @param pre_lv 数组，长度为 (m_level +1).
     *      可以是 NULL
     */
    Node* searchNode(const T& k, Node **pre_lv)
    {
        Node *ret = NULL, *pre = NULL;
        int lv = m_level;
        do
        {
            while (true)
            {
                Node *n = (NULL == pre ? m_head[lv] : pre->m_next[lv]);
                if (NULL == n)
                {
                    if (NULL != pre_lv)
                        pre_lv[lv] = pre;
                    break;
                }

                if (k < n->m_key)
                {
                    if (NULL != pre_lv)
                        pre_lv[lv] = pre;
                    break;
                }
                else if (k == n->m_key)
                {
                    if (NULL == pre_lv)
                    {
                        return n; // 找到节点直接返回
                    }
                    else
                    {
                        pre_lv[lv] = pre;
                        ret = n;
                        break; // 即使找到，也继续找，以便填充完 pre_lv[]
                    }
                }
                else
                {
                    pre = n;
                }
            }
        } while (--lv >= 0);
        return ret;
    }

    /**
     * 插入节点
     *
     * @param n 要插入的节点. 
     *      如果其 m_next 为 NULL，则会随机生成level数。否则使用已有的m_level数和m_next数组
     */
    void insertNode(Node *n, Node** pre_lv)
    {
        assert(NULL != n && NULL != pre_lv);

        // random level
        if (NULL == n->m_next)
        {
            n->m_level = randomLevel();
            n->m_next = (Node **) ::malloc(sizeof(Node*) * (n->m_level + 1));
        }

        // adjust low-half level
        for (register size_t i = 0; i <= m_level && i <= n->m_level; ++i)
        {
            if (NULL == pre_lv[i])
            {
                n->m_next[i] = m_head[i];
                m_head[i] = n;
            }
            else
            {
                n->m_next[i] = pre_lv[i]->m_next[i];
                pre_lv[i]->m_next[i] = n;
            }
        }

        // adjust high-half level
        if (n->m_level > m_level)
        {
            m_head = (Node**) ::realloc(m_head, sizeof(Node*) * (n->m_level + 1));
            for (register size_t i = m_level + 1; i <= n->m_level; ++i)
            {
                m_head[i] = n;
                n->m_next[i] = NULL;
            }
            m_level = level;
        }
    }

public:
    SkipListSet()
        : m_level(0), m_head(NULL), m_size(0)
    {
        m_head = (Node**) ::malloc(sizeof(Node*) * 1);
        ::memset(m_head, 0, sizeof(Node*) * 1);
    }

    SkipListSet(const SkipListSet<T>& x)
        : m_level(0), m_head(NULL), m_size(0)
    {
        // initial attributes
        m_level = x.m_level;
        m_head = (Node**) ::malloc(sizeof(Node*) * (m_level + 1));
        ::memset(m_head, 0, sizeof(Node*) * (m_level + 1));

        // copy nodes
        Node **pre_lv = (Node**) ::malloc(sizeof(Node*) * (m_level + 1));
        ::memset(pre_lv, 0, sizeof(Node*) * (m_level + 1));
        Node *n = x.m_head[0];
        while (NULL != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) (n->m_key);
            c->m_level = n->m_level;
            c->m_next = (Node**) ::malloc(sizeof(Node*) * (c->m_level + 1));
            insertNode(c, pre_lv);
            for (register size_t i = 0; i <= c->m_level; ++i)
                pre_lv[i] = c;

            n = n->m_next[0];
        }
        ::free(pre_lv);
        m_size = x.m_size;

        return *this;
    }

    ~SkipListSet()
    {
        clear();
        ::free(m_head);
        m_head = NULL;
    }

    SkipListSet<T>& operator=(const SkipListSet<T>& x)
    {
        if (this == &x)
            return *this;

        // clear memory
        clear();
        ::free(m_head);

        // initial attributes
        m_level = x.m_level;
        m_head = (Node**) ::malloc(sizeof(Node*) * (m_level + 1));
        ::memset(m_head, 0, sizeof(Node*) * (m_level + 1));

        // copy nodes
        Node **pre_lv = (Node**) ::malloc(sizeof(Node*) * (m_level + 1));
        ::memset(pre_lv, 0, sizeof(Node*) * (m_level + 1));
        Node *n = x.m_head[0];
        while (NULL != n)
        {
            Node *c = (Node*) ::malloc(sizeof(Node));
            new (c) (n->m_key);
            c->m_level = n->m_level;
            c->m_next = (Node**) ::malloc(sizeof(Node*) * (c->m_level + 1));
            insertNode(c, pre_lv);
            for (register size_t i = 0; i <= c->m_level; ++i)
                pre_lv[i] = c;

            n = n->m_next[0];
        }
        ::free(pre_lv);
        m_size = x.m_size;

        return *this;
    }

    bool operator==(const SkipListSet<T>& x) const
    {
        if (this == &x)
            return true;
        if (m_size != x.m_size)
            return false;

        Node *current1 = m_head[0], current2 = x.m_head[0];
        while (NULL != current)
        {
            if (current1->m_key != current2->m_key)
                return false;
            current1 = current1->m_next[0];
            current2 = current2->m_next[0];
        }
        return true;
    }

    bool operator!=(const SkipListSet<T>& x) const
    {
        return !(*this == x);
    }

    size_t size() const
    {
        return m_size;
    }

    /**
     * 层数
     *
     * @return 1-based
     */
    size_t level() const
    {
        int ret = m_level;
        while (ret > 0 && NULL != m_head[ret])
            --ret;
        return ret + 1;
    }

    void clear()
    {
        Node *current = m_head[0];
        while (NULL != current)
        {
            Node *next = current->m_next[0];
            ::free(current->m_next);
            ::free(current);
            current = next;
        }
        ::memset(m_head, 0, sizeof(Node*) * (m_level + 1));
        m_size = 0;
    }

    public bool contains(const T& k) const
    {
        return NULL != searchNode(k, NULL);
    }

    public bool add(const T& k)
    {
        // search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (m_level + 1));
        Node *n = searchNode(k, pre_lv);
        if (NULL != n)
        {
            ::free(pre_lv);
            return false;
        }

        // insert
        n = (Node*) ::malloc(sizeof(Node));
        new (n) (k);
        insertNode(n, pre_lv);
        ::free(pre_lv);
        ++m_size;
        return true;
    }

    public bool remove(const T& k)
    {
        // search
        Node **pre_lv = (Node **) ::malloc(sizeof(Node*) * (m_level + 1));
        Node *n = searchNode(k, pre_lv);
        if (NULL == n)
        {
            ::free(pre_lv);
            return false;
        }

        // remove
        for (register size_t i = 0; i <= n->m_level; ++i)
        {
            if (NULL == pre_lv[i])
                m_head[i] = n->m_next[i];
            else
                pre_lv[i]->m_next[i] = n->m_next[i];
        }
        ::free(n->m_next);
        ::free(n);
        --m_size;
        return true;
    }
};

}

#endif
