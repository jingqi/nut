/**
 * @file -
 * @author jingqi
 * @date 2012-08-10
 * @last-edit 2015-01-06 23:53:11 jingqi
 */

#ifndef ___HEADFILE_A7E2D32B_B83E_44AB_A6C6_98E03E0EDDBD_
#define ___HEADFILE_A7E2D32B_B83E_44AB_A6C6_98E03E0EDDBD_

#include <assert.h>
#include <sqlite3.h>
#include <vector>

#include <nut/gc/ref.hpp>
#include <nut/debugging/exception.hpp>

#include "preparedstatement.hpp"
#include "resultset.hpp"

namespace nut
{

class Connection
{
    NUT_GC_REFERABLE

    sqlite3 *m_sqlite;
    bool m_auto_commit;
    bool m_throw_exceptions;
    int m_last_error;
    std::string m_last_error_msg;

    struct Sqlite3Freer
    {
        void *pt;

        Sqlite3Freer(void *p = NULL) : pt(p) {}
        void attach(void *p) { pt = p; }
        ~Sqlite3Freer()
        {
            if (NULL != pt)
                ::sqlite3_free(pt);
        }
    };

    void on_error(int err, const char *msg = NULL)
    {
        m_last_error = err;

        if (NULL == msg)
        {
            assert(NULL != m_sqlite);
            msg = ::sqlite3_errmsg(m_sqlite); // XXX memory of "msg" is managed internally by sqlite3
        }
        m_last_error_msg = (NULL == msg ? "no error detected" : msg);

        if (m_throw_exceptions)
            throw ExceptionA(m_last_error, m_last_error_msg, __FILE__, __LINE__, __FUNCTION__);
    }

public:
    Connection()
        : m_sqlite(NULL), m_auto_commit(true), m_throw_exceptions(false),
          m_last_error(SQLITE_OK)
    {}

    Connection(sqlite3 *db)
        : m_sqlite(db), m_auto_commit(true), m_throw_exceptions(false),
          m_last_error(SQLITE_OK)
    {
        assert(NULL != db);
    }

    /**
     * @param dbfilepath File path encoded in UTF-8
     */
    Connection(const char *dbfilepath)
        : m_sqlite(NULL), m_auto_commit(true), m_throw_exceptions(false),
          m_last_error(SQLITE_OK)
    {
        assert(NULL != dbfilepath);
        open(dbfilepath);
    }

    ~Connection()
    {
        bool rs = close();
        assert(rs);
    }

    bool open(const char *dbfilepath)
    {
        assert(NULL != dbfilepath);

        if (is_valid())
        {
            bool rs = close();
            assert(rs);
        }

        int rs = ::sqlite3_open(dbfilepath, &m_sqlite);
        if (SQLITE_OK != rs)
        {
            m_sqlite = NULL;
            on_error(rs, "open db file failed");
            return false;
        }
        assert(is_valid());
        return true;
    }

    bool close()
    {
        if (NULL == m_sqlite)
            return true;

        int rs = ::sqlite3_close(m_sqlite);
        if (SQLITE_OK != rs)
        {
            on_error(rs);
            return false;
        }
        else
        {
            m_sqlite = NULL;
            return true;
        }
    }

    inline bool is_valid() const
    {
        return NULL != m_sqlite;
    }

    inline bool is_auto_commit() const
    {
        return m_auto_commit;
    }

    inline void set_auto_commit(bool b)
    {
        m_auto_commit = b;
    }

    inline bool is_throw_exceptions() const
    {
        return m_throw_exceptions;
    }

    inline void set_throw_exceptions(bool b)
    {
        m_throw_exceptions = b;
    }

    const std::string& get_lastError_msg() const
    {
        return m_last_error_msg;
    }

    bool start()
    {
        assert(is_valid());
        char *msg = NULL;
        int rs = ::sqlite3_exec(m_sqlite, "begin;", NULL, NULL, &msg);
        Sqlite3Freer _f(msg);
        if (SQLITE_OK != rs)
        {
            on_error(rs, msg);
            return false;
        }
        return true;
    }

    bool commit()
    {
        assert(is_valid());
        char *msg = NULL;
        int rs = ::sqlite3_exec(m_sqlite, "commit;", NULL, NULL, &msg);
        Sqlite3Freer _f(msg);
        if (SQLITE_OK != rs)
        {
            on_error(rs, msg);
            return false;
        }
        return true;
    }

    bool rollback()
    {
        assert(is_valid());
        char *msg = NULL;
        int rs = ::sqlite3_exec(m_sqlite, "rollback;", NULL, NULL, &msg);
        Sqlite3Freer _f(msg);
        if (SQLITE_OK != rs)
        {
            on_error(rs, msg);
            return false;
        }
        return true;
    }

    /** 压缩数据库 */
    bool vacuum()
    {
        assert(is_valid());
        char *msg = NULL;
        int rs = ::sqlite3_exec(m_sqlite, "vacuum;", NULL, NULL, &msg);
        Sqlite3Freer _f(msg);
        if (SQLITE_OK != rs)
        {
            on_error(rs, msg);
            return false;
        }
        return true;
    }

    bool execute_update(const char *sql)
    {
        assert(NULL != sql && is_valid());
        char *msg = NULL;
        if (m_auto_commit)
            start();
        int rs = ::sqlite3_exec(m_sqlite, sql, NULL, NULL, &msg);
        Sqlite3Freer _f(msg);
        if (SQLITE_OK != rs)
        {
            if (m_auto_commit)
                rollback();
            on_error(rs, msg);
            return false;
        }
        if (m_auto_commit)
            commit();
        return true;
    }

    bool execute_update(const char *sql, const ParamWraper& arg1,
        const ParamWraper& arg2 = ParamWraper::none(),
        const ParamWraper& arg3 = ParamWraper::none(),
        const ParamWraper& arg4 = ParamWraper::none(),
        const ParamWraper& arg5 = ParamWraper::none(),
        const ParamWraper& arg6 = ParamWraper::none(),
        const ParamWraper& arg7 = ParamWraper::none(),
        const ParamWraper& arg8 = ParamWraper::none(),
        const ParamWraper& arg9 = ParamWraper::none())
    {
        assert(NULL != sql && is_valid());

        // 预编译
        ref<PreparedStatement> stmt = gc_new<PreparedStatement>(m_sqlite, sql);
        if (!stmt->is_valid())
        {
            on_error(SQLITE_ERROR);
            return false;
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            on_error(SQLITE_ERROR);
            return false;
        }

#define __BIND(i) \
        rs = stmt->bind(i, arg##i); \
        if (!rs) \
        { \
            on_error(SQLITE_ERROR); \
            return false; \
        }

        __BIND(1)
        __BIND(2)
        __BIND(3)
        __BIND(4)
        __BIND(5)
        __BIND(6)
        __BIND(7)
        __BIND(8)
        __BIND(9)

#undef __BIND

        // 执行
        if (m_auto_commit)
            start();
        int irs = ::sqlite3_step(stmt->stmt()->raw());
        if (SQLITE_DONE != irs)
        {
            if (m_auto_commit)
                rollback();
            on_error(irs);
            return false;
        }
        if (m_auto_commit)
            commit();
        return true;
    }

    bool execute_update(const char *sql, const std::vector<ParamWraper>& args)
    {
        assert(NULL != sql && is_valid());

        // 预编译
        ref<PreparedStatement> stmt = gc_new<PreparedStatement>(m_sqlite, sql);
        if (!stmt->is_valid())
        {
            on_error(SQLITE_ERROR);
            return false;
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            on_error(SQLITE_ERROR);
            return false;
        }
        for (size_t i = 0, size = args.size(); i < size; ++i)
        {
            rs = stmt->bind(i + 1, args.at(i));
            if (!rs)
            {
                on_error(SQLITE_ERROR);
                return false;
            }
        }
            // 执行
        if (m_auto_commit)
             start();
        int irs = ::sqlite3_step(stmt->stmt()->raw());
        if (SQLITE_DONE != irs)
        {
            if (m_auto_commit)
                rollback();
            on_error(irs);
            return false;
        }
        if (m_auto_commit)
            commit();
        return true;
    }

    ref<ResultSet> execute_query(const char *sql,
        const ParamWraper& arg1 = ParamWraper::none(),
        const ParamWraper& arg2 = ParamWraper::none(),
        const ParamWraper& arg3 = ParamWraper::none(),
        const ParamWraper& arg4 = ParamWraper::none(),
        const ParamWraper& arg5 = ParamWraper::none(),
        const ParamWraper& arg6 = ParamWraper::none(),
        const ParamWraper& arg7 = ParamWraper::none(),
        const ParamWraper& arg8 = ParamWraper::none(),
        const ParamWraper& arg9 = ParamWraper::none())
    {
        assert(NULL != sql && is_valid());

        // 预编译
        ref<PreparedStatement> stmt = gc_new<PreparedStatement>(m_sqlite, sql);
        if (!stmt->is_valid())
        {
            on_error(SQLITE_ERROR);
            return gc_new<ResultSet>();
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            on_error(SQLITE_ERROR);
            return gc_new<ResultSet>();
        }

#define __BIND(i) \
        rs = stmt->bind(i, arg##i); \
        if (!rs) \
        { \
            on_error(SQLITE_ERROR); \
            return gc_new<ResultSet>(); \
        }

        __BIND(1)
        __BIND(2)
        __BIND(3)
        __BIND(4)
        __BIND(5)
        __BIND(6)
        __BIND(7)
        __BIND(8)
        __BIND(9)

#undef __BIND

        // 执行
        return gc_new<ResultSet>(stmt->stmt());
    }

    ref<ResultSet> execute_query(const char *sql, const std::vector<ParamWraper> args)
    {
        assert(NULL != sql && is_valid());

        // 预编译
        ref<PreparedStatement> stmt = gc_new<PreparedStatement>(m_sqlite, sql);
        if (!stmt->is_valid())
        {
            on_error(SQLITE_ERROR);
            return gc_new<ResultSet>();
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            on_error(SQLITE_ERROR);
            return gc_new<ResultSet>();
        }
        for (size_t i = 0, size = args.size(); i < size; ++i)
        {
            rs = stmt->bind(i + 1, args.at(i));
            if (!rs)
            {
                on_error(SQLITE_ERROR);
                return gc_new<ResultSet>();
            }
        }

        // 执行
        return gc_new<ResultSet>(stmt->stmt());
    }
};

}

#endif
