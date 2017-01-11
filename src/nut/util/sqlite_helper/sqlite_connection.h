
#ifndef ___HEADFILE_A7E2D32B_B83E_44AB_A6C6_98E03E0EDDBD_
#define ___HEADFILE_A7E2D32B_B83E_44AB_A6C6_98E03E0EDDBD_

#include <assert.h>
#include <sqlite3.h>
#include <vector>

#include <nut/platform/platform.h>
#include <nut/rc/rc_ptr.h>
#include <nut/debugging/exception.h>

#include "sqlite_param.h"
#include "sqlite_statement.h"
#include "sqlite_result_set.h"

namespace nut
{

class SqliteConnection
{
    NUT_REF_COUNTABLE

    sqlite3 *_sqlite = nullptr;
    bool _auto_commit = true;
    bool _throw_exceptions = false;
    int _last_error = SQLITE_OK;
    std::string _last_error_msg;

    class Sqlite3Freer
    {
        void *_ptr = nullptr;

    public:
        Sqlite3Freer(void *p = nullptr)
            : _ptr(p)
        {}

        void attach(void *p)
        {
            _ptr = p;
        }

        ~Sqlite3Freer()
        {
            if (nullptr != _ptr)
                ::sqlite3_free(_ptr);
        }
    };

    void on_error(int err = SQLITE_OK, const char *msg = nullptr)
    {
        _last_error = err;
        if (SQLITE_OK == err && nullptr != _sqlite)
            _last_error = ::sqlite3_errcode(_sqlite);

        if (nullptr == msg)
        {
            if (nullptr != _sqlite)
                msg = ::sqlite3_errmsg(_sqlite); // XXX memory of "msg" is managed internally by sqlite3
            else if (SQLITE_OK != _last_error)
                msg = ::sqlite3_errstr(_last_error);
        }
        _last_error_msg = (nullptr == msg ? "no error detected" : msg);

        if (_throw_exceptions)
            throw ExceptionA(_last_error, _last_error_msg, __FILE__, __LINE__, __FUNCTION__);
    }

public:
    SqliteConnection() = default;

    SqliteConnection(sqlite3 *db)
        : _sqlite(db)
    {
        assert(nullptr != db);
    }

    /**
     * @param dbfilepath File path encoded in UTF-8
     */
    SqliteConnection(const char *db_file)
    {
        assert(nullptr != db_file);
        open(db_file);
    }

    ~SqliteConnection()
    {
        bool rs = close();
        assert(rs);
        UNUSED(rs);
    }

    bool open(const char *db_file)
    {
        assert(nullptr != db_file);

        // Close old database if exists
        if (is_valid())
        {
            bool rs = close();
            assert(rs);
            UNUSED(rs);
        }

        // Open new database
        _last_error = SQLITE_OK;
        _last_error_msg.clear();
        _sqlite = nullptr;
        const int rs = ::sqlite3_open(db_file, &_sqlite);
        if (SQLITE_OK != rs)
        {
            on_error(rs);
            _sqlite = nullptr;
            return false;
        }
        assert(is_valid());
        return SQLITE_OK == rs;
    }

    bool close()
    {
        if (nullptr == _sqlite)
            return true;

        const int rs = ::sqlite3_close(_sqlite);
        if (SQLITE_OK != rs)
            on_error(rs);
        else
            _sqlite = nullptr;
        return SQLITE_OK == rs;
    }

#ifdef SQLITE_HAS_CODEC
    /**
     * 设置加密密码，或者用密码打开已加密数据库
     */
    bool set_key(const char *key, int key_len)
    {
        assert(is_valid());
        const int rs = ::sqlite3_key(_sqlite, key, key_len);
        if (SQLITE_OK != rs)
            on_error(rs);
        return SQLITE_OK == rs;
    }

    /**
     * 更改密数据库码
     */
    bool change_key(const char *key, int key_len)
    {
        assert(is_valid());
        const int rs = ::sqlite3_rekey(_sqlite, key, key_len);
        if (SQLITE_OK != rs)
            on_error(rs);
        return SQLITE_OK == rs;
    }
#endif

    sqlite3* get_raw_db() const
    {
        return _sqlite;
    }

    bool is_valid() const
    {
        return nullptr != _sqlite;
    }

    bool is_auto_commit() const
    {
        return _auto_commit;
    }

    void set_auto_commit(bool b)
    {
        _auto_commit = b;
    }

    bool is_throw_exceptions() const
    {
        return _throw_exceptions;
    }

    void set_throw_exceptions(bool b)
    {
        _throw_exceptions = b;
    }

    int get_last_error_code() const
    {
        return _last_error;
    }

    const std::string& get_last_error_msg() const
    {
        return _last_error_msg;
    }

    bool start()
    {
        assert(is_valid());
        char *msg = nullptr;
        const int rs = ::sqlite3_exec(_sqlite, "begin;", nullptr, nullptr, &msg);
        Sqlite3Freer _g(msg);
        if (SQLITE_OK != rs)
            on_error(rs, msg);
        return SQLITE_OK == rs;
    }

    bool commit()
    {
        assert(is_valid());
        char *msg = nullptr;
        const int rs = ::sqlite3_exec(_sqlite, "commit;", nullptr, nullptr, &msg);
        Sqlite3Freer _g(msg);
        if (SQLITE_OK != rs)
            on_error(rs, msg);
        return SQLITE_OK == rs;
    }

    bool rollback()
    {
        assert(is_valid());
        char *msg = nullptr;
        const int rs = ::sqlite3_exec(_sqlite, "rollback;", nullptr, nullptr, &msg);
        Sqlite3Freer _g(msg);
        if (SQLITE_OK != rs)
            on_error(rs, msg);
        return SQLITE_OK == rs;
    }

    /** 压缩数据库 */
    bool vacuum()
    {
        assert(is_valid());
        char *msg = nullptr;
        const int rs = ::sqlite3_exec(_sqlite, "vacuum;", nullptr, nullptr, &msg);
        Sqlite3Freer _g(msg);
        if (SQLITE_OK != rs)
            on_error(rs, msg);
        return SQLITE_OK == rs;
    }

    bool execute_update(const char *sql)
    {
        assert(nullptr != sql && is_valid());
        char *msg = nullptr;
        if (_auto_commit)
            start();
        const int rs = ::sqlite3_exec(_sqlite, sql, nullptr, nullptr, &msg);
        Sqlite3Freer _g(msg);
        if (SQLITE_OK != rs)
        {
            if (_auto_commit)
                rollback();
            on_error(rs, msg);
            return false;
        }
        if (_auto_commit)
            commit();
        return true;
    }

    bool execute_update(const char *sql, const SqliteParam& arg1,
        const SqliteParam& arg2 = SqliteParam::none(),
        const SqliteParam& arg3 = SqliteParam::none(),
        const SqliteParam& arg4 = SqliteParam::none(),
        const SqliteParam& arg5 = SqliteParam::none(),
        const SqliteParam& arg6 = SqliteParam::none(),
        const SqliteParam& arg7 = SqliteParam::none(),
        const SqliteParam& arg8 = SqliteParam::none(),
        const SqliteParam& arg9 = SqliteParam::none())
    {
        assert(nullptr != sql && is_valid());

        // 预编译
        rc_ptr<SqliteStatement> stmt = rc_new<SqliteStatement>(_sqlite, sql);
        if (!stmt->is_valid())
        {
            on_error();
            return false;
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            on_error();
            return false;
        }

#define __BIND(i)                   \
        rs = stmt->bind(i, arg##i); \
        if (!rs)                    \
        {                           \
            on_error();             \
            return false;           \
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
        if (_auto_commit)
            start();
        const int irs = ::sqlite3_step(stmt->get_raw_stmt());
        if (SQLITE_DONE != irs)
        {
            if (_auto_commit)
                rollback();
            on_error(irs);
            return false;
        }
        if (_auto_commit)
            commit();
        return true;
    }

    bool execute_update(const char *sql, const std::vector<SqliteParam>& args)
    {
        assert(nullptr != sql && is_valid());

        // 预编译
        rc_ptr<SqliteStatement> stmt = rc_new<SqliteStatement>(_sqlite, sql);
        if (!stmt->is_valid())
        {
            on_error();
            return false;
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            on_error();
            return false;
        }
        for (size_t i = 0, size = args.size(); i < size; ++i)
        {
            rs = stmt->bind(i + 1, args.at(i));
            if (!rs)
            {
                on_error();
                return false;
            }
        }
            // 执行
        if (_auto_commit)
             start();
        const int irs = ::sqlite3_step(stmt->get_raw_stmt());
        if (SQLITE_DONE != irs)
        {
            if (_auto_commit)
                rollback();
            on_error(irs);
            return false;
        }
        if (_auto_commit)
            commit();
        return true;
    }

    rc_ptr<SqliteResultSet> execute_query(const char *sql,
        const SqliteParam& arg1 = SqliteParam::none(),
        const SqliteParam& arg2 = SqliteParam::none(),
        const SqliteParam& arg3 = SqliteParam::none(),
        const SqliteParam& arg4 = SqliteParam::none(),
        const SqliteParam& arg5 = SqliteParam::none(),
        const SqliteParam& arg6 = SqliteParam::none(),
        const SqliteParam& arg7 = SqliteParam::none(),
        const SqliteParam& arg8 = SqliteParam::none(),
        const SqliteParam& arg9 = SqliteParam::none())
    {
        assert(nullptr != sql && is_valid());

        // 预编译
        rc_ptr<SqliteStatement> stmt = rc_new<SqliteStatement>(_sqlite, sql);
        if (!stmt->is_valid())
        {
            on_error();
            return rc_new<SqliteResultSet>();
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            on_error();
            return rc_new<SqliteResultSet>();
        }

#define __BIND(i)                             \
        rs = stmt->bind(i, arg##i);           \
        if (!rs)                              \
        {                                     \
            on_error();                       \
            return rc_new<SqliteResultSet>(); \
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
        return rc_new<SqliteResultSet>(stmt);
    }

    rc_ptr<SqliteResultSet> execute_query(const char *sql, const std::vector<SqliteParam>& args)
    {
        assert(nullptr != sql && is_valid());

        // 预编译
        rc_ptr<SqliteStatement> stmt = rc_new<SqliteStatement>(_sqlite, sql);
        if (!stmt->is_valid())
        {
            on_error();
            return rc_new<SqliteResultSet>();
        }

        // 绑定参数
        bool rs = stmt->reset();
        if (!rs)
        {
            on_error();
            return rc_new<SqliteResultSet>();
        }
        for (size_t i = 0, size = args.size(); i < size; ++i)
        {
            rs = stmt->bind(i + 1, args.at(i));
            if (!rs)
            {
                on_error();
                return rc_new<SqliteResultSet>();
            }
        }

        // 执行
        return rc_new<SqliteResultSet>(stmt);
    }
};

}

#endif
