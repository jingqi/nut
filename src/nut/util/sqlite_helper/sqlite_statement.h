
#ifndef ___HEADFILE_54107F5A_983A_4021_AB0E_0E5422FE5FD9_
#define ___HEADFILE_54107F5A_983A_4021_AB0E_0E5422FE5FD9_

#include <vector>
#include <string>
#include <sqlite3.h>

#include <nut/platform/platform.h>
#include <nut/rc/rc_new.h>
#include <nut/rc/enrc.h>

#include "sqlite_param.h"

namespace nut
{

class SqliteStatement
{
    NUT_REF_COUNTABLE

    sqlite3_stmt *_stmt = NULL;
    std::vector<rc_ptr<enrc<std::string> > > _strings;

public:
    SqliteStatement()
    {}

    SqliteStatement(sqlite3 *db, const char *sql)
    {
        assert(NULL != db && NULL != sql);
        prepare(db, sql);
    }

    ~SqliteStatement()
    {
        clear();
    }

    bool prepare(sqlite3 *db, const char *sql)
    {
        assert(NULL != db && NULL != sql);
        clear();
        const int rs = ::sqlite3_prepare_v2(db, sql, -1, &_stmt, NULL);
        if (SQLITE_OK != rs || NULL == _stmt)
            return false;
        return true;
    }

    bool is_valid() const
    {
        return NULL != _stmt;
    }

    bool reset()
    {
        assert(is_valid());
        _strings.clear();
        return SQLITE_OK == ::sqlite3_reset(_stmt);
    }

    void clear()
    {
        _strings.clear();
        if (NULL != _stmt)
        {
            const int rs = ::sqlite3_finalize(_stmt);
            assert(SQLITE_OK == rs);
            UNUSED(rs);
            _stmt = NULL;
        }
    }

    /**
     * @param pos 1-based
     */
    bool bind(int pos, const SqliteParam& param)
    {
        assert(is_valid());
        switch (param.type)
        {
        case SqliteParam::INTEGER:
            return SQLITE_OK == ::sqlite3_bind_int(_stmt, pos, param.int_arg);

        case SqliteParam::STRING:
            // NOTE sqlite3 只有在用到字符串时才会去取值，这里需要缓存一下字符串
            //      否则导致访问内存错误，或者其他未定义错误
            assert(!param.string_arg.is_null());
            _strings.push_back(param.string_arg);
            return SQLITE_OK == ::sqlite3_bind_text(_stmt, pos, param.string_arg->c_str(), -1, NULL);

        default:
            return true;
        }
    }

    sqlite3_stmt* get_raw_stmt()
    {
        return _stmt;
    }
};

}

#endif
