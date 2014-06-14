/**
 * @file -
 * @author jingqi
 * @date 2012-08-10
 * @last-edit 2012-08-10 22:54:30 jingqi
 */

#ifndef ___HEADFILE_A7E2D32B_B83E_44AB_A6C6_98E03E0EDDBD_
#define ___HEADFILE_A7E2D32B_B83E_44AB_A6C6_98E03E0EDDBD_

#include <assert.h>
#include <sqlite3.h>
#include <vector>

#include <nut/gc/ref.hpp>
#include <nut/debugging/exception.hpp>

#include "preparedstatement .hpp"
#include "resultset.hpp"

namespace nut
{

class Connection
{
    NUT_GC_REFERABLE

    sqlite3 *m_sqlite;
    bool m_autoCommit;
    bool m_throwExceptions;
    int m_lastError;
    std::string m_lastErrorMsg;

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

    void onError(int err, const char *msg = NULL)
    {
        m_lastError = err;

        if (NULL != msg)
        {
            m_lastErrorMsg = msg;
        }
        else
        {
            assert(NULL != m_sqlite);
            msg = ::sqlite3_errmsg(m_sqlite);
            Sqlite3Freer _f((void*) msg);
            if (NULL == msg)
                msg = "no error";
            m_lastErrorMsg = msg;
        }

        if (m_throwExceptions)
            throw ExceptionA(m_lastError, m_lastErrorMsg, __FILE__, __LINE__, __FUNCTION__);
    }

public:
    Connection()
        : m_sqlite(NULL), m_autoCommit(true), m_throwExceptions(false),
          m_lastError(SQLITE_OK)
    {}

    Connection(sqlite3 *db)
        : m_sqlite(db), m_autoCommit(true), m_throwExceptions(false),
          m_lastError(SQLITE_OK)
    {
        assert(NULL != db);
    }

    /**
     * @param dbfilepath File path encoded in UTF-8
     */
    Connection(const char *dbfilepath)
        : m_sqlite(NULL), m_autoCommit(true), m_throwExceptions(false),
          m_lastError(SQLITE_OK)
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

        if (isValid())
        {
            bool rs = close();
            assert(rs);
        }

        int rs = ::sqlite3_open(dbfilepath, &m_sqlite);
        if (SQLITE_OK != rs)
        {
            m_sqlite = NULL;
            onError(rs, "open db file failed");
            return false;
        }
        assert(isValid());
        return true;
    }

    bool close()
    {
        if (NULL == m_sqlite)
            return true;

        int rs = ::sqlite3_close(m_sqlite);
        if (SQLITE_OK != rs)
        {
            onError(rs);
            return false;
        }
        else
        {
            m_sqlite = NULL;
            return true;
        }
    }

    inline bool isValid() const
    {
        return NULL != m_sqlite;
    }

    inline bool isAutoCommit() const
    {
        return m_autoCommit;
    }

    inline void setAutoCommit(bool b)
    {
        m_autoCommit = b;
    }
    
    inline bool isThrowExceptions() const
    {
        return m_throwExceptions;
    }

    inline void setThrowExceptions(bool b)
    {
        m_throwExceptions = b;
    }

    std::string getLastErrorMsg() const
    {
        return m_lastErrorMsg;
    }

    bool start()
    {
        assert(isValid());
        char *msg = NULL;
        int rs = ::sqlite3_exec(m_sqlite, "begin;", NULL, NULL, &msg);
        Sqlite3Freer _f(msg);
        if (SQLITE_OK != rs)
        {
            onError(rs, msg);
            return false;
        }
        return true;
    }

    bool commit()
    {
        assert(isValid());
        char *msg = NULL;
        int rs = ::sqlite3_exec(m_sqlite, "commit;", NULL, NULL, &msg);
        Sqlite3Freer _f(msg);
        if (SQLITE_OK != rs)
        {
            onError(rs, msg);
            return false;
        }
        return true;
    }

    bool rollback()
    {
        assert(isValid());
        char *msg = NULL;
        int rs = ::sqlite3_exec(m_sqlite, "rollback;", NULL, NULL, &msg);
        Sqlite3Freer _f(msg);
        if (SQLITE_OK != rs)
        {
            onError(rs, msg);
            return false;
        }
        return true;
    }

    /** 压缩数据库 */
    bool vacuum()
    {
        assert(isValid());
        char *msg = NULL;
        int rs = ::sqlite3_exec(m_sqlite, "vacuum;", NULL, NULL, &msg);
        Sqlite3Freer _f(msg);
        if (SQLITE_OK != rs)
        {
            onError(rs, msg);
            return false;
        }
        return true;
    }

    bool executeUpdate(const char *sql)
    {
        assert(NULL != sql && isValid());
        char *msg = NULL;
        if (m_autoCommit)
            start();
        int rs = ::sqlite3_exec(m_sqlite, sql, NULL, NULL, &msg);
        Sqlite3Freer _f(msg);
        if (SQLITE_OK != rs)
        {
            if (m_autoCommit)
                rollback();
            onError(rs, msg);
            return false;
        }
        if (m_autoCommit)
            commit();
        return true;
    }

    bool executeUpdate(const char *sql, const ParamWraper& arg1,
        const ParamWraper& arg2 = ParamWraper::none(),
        const ParamWraper& arg3 = ParamWraper::none(),
        const ParamWraper& arg4 = ParamWraper::none(),
        const ParamWraper& arg5 = ParamWraper::none(),
        const ParamWraper& arg6 = ParamWraper::none())
    {
        assert(NULL != sql && isValid());

        // 预编译
        ref<PreparedStatement> stmt = gc_new<PreparedStatement>(m_sqlite, sql);
        if (!stmt->isValid())
        {
            onError(SQLITE_ERROR);
            return false;
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            onError(SQLITE_ERROR);
            return false;
        }

#define __BIND(i) \
        rs = stmt->bind(i, arg##i); \
        if (!rs) \
        { \
            onError(SQLITE_ERROR); \
            return false; \
        }

        __BIND(1)
        __BIND(2)
        __BIND(3)
        __BIND(4)
        __BIND(5)
        __BIND(6)

#undef __BIND

        // 执行
        if (m_autoCommit)
            start();
        int irs = ::sqlite3_step(stmt->stmt()->raw());
        if (SQLITE_DONE != irs)
        {
            if (m_autoCommit)
                rollback();
            onError(irs);
            return false;
        }
        if (m_autoCommit)
            commit();
        return true;
    }

    bool executeUpdate(const char *sql, const std::vector<ParamWraper>& args)
    {
        assert(NULL != sql && isValid());

        // 预编译
        ref<PreparedStatement> stmt = gc_new<PreparedStatement>(m_sqlite, sql);
        if (!stmt->isValid())
        {
            onError(SQLITE_ERROR);
            return false;
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            onError(SQLITE_ERROR);
            return false;
        }
        for (register size_t i = 0, size = args.size(); i < size; ++i)
        {
            rs = stmt->bind(i + 1, args.at(i));
            if (!rs)
            {
                onError(SQLITE_ERROR);
                return false;
            }
        }
            // 执行
        if (m_autoCommit)
             start();
        int irs = ::sqlite3_step(stmt->stmt()->raw());
        if (SQLITE_DONE != irs)
        {
            if (m_autoCommit)
                rollback();
            onError(irs);
            return false;
        }
        if (m_autoCommit)
            commit();
        return true;
    }

    ref<ResultSet> executeQuery(const char *sql,
        const ParamWraper& arg1 = ParamWraper::none(),
        const ParamWraper& arg2 = ParamWraper::none(),
        const ParamWraper& arg3 = ParamWraper::none(),
        const ParamWraper& arg4 = ParamWraper::none(),
        const ParamWraper& arg5 = ParamWraper::none(),
        const ParamWraper& arg6 = ParamWraper::none())
    {
        assert(NULL != sql && isValid());

        // 预编译
        ref<PreparedStatement> stmt = gc_new<PreparedStatement>(m_sqlite, sql);
        if (!stmt->isValid())
        {
            onError(SQLITE_ERROR);
            return gc_new<ResultSet>();
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            onError(SQLITE_ERROR);
            return gc_new<ResultSet>();
        }

#define __BIND(i) \
        rs = stmt->bind(i, arg##i); \
        if (!rs) \
        { \
            onError(SQLITE_ERROR); \
            return gc_new<ResultSet>(); \
        }

        __BIND(1)
        __BIND(2)
        __BIND(3)
        __BIND(4)
        __BIND(5)
        __BIND(6)

#undef __BIND

        // 执行
        return gc_new<ResultSet>(stmt->stmt());
    }

    ref<ResultSet> executeQuery(const char *sql, const std::vector<ParamWraper> args)
    {
        assert(NULL != sql && isValid());

        // 预编译
        ref<PreparedStatement> stmt = gc_new<PreparedStatement>(m_sqlite, sql);
        if (!stmt->isValid())
        {
            onError(SQLITE_ERROR);
            return gc_new<ResultSet>();
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            onError(SQLITE_ERROR);
            return gc_new<ResultSet>();
        }
        for (register size_t i = 0, size = args.size(); i < size; ++i)
        {
            rs = stmt->bind(i + 1, args.at(i));
            if (!rs)
            {
                onError(SQLITE_ERROR);
                return gc_new<ResultSet>();
            }
        }

        // 执行
        return gc_new<ResultSet>(stmt->stmt());
    }
};

}

#endif

