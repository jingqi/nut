
#ifndef ___HEADFILE_B51CBEE3_B5FB_4481_AE29_559B709E3B26_
#define ___HEADFILE_B51CBEE3_B5FB_4481_AE29_559B709E3B26_

#include <assert.h>
#include <string>
#include <string.h>
#include <sqlite3.h>

#include <nut/rc/rc_new.h>
#include <nut/util/string/string_util.h>

#include "sqlite_statement.h"

namespace nut
{

class SqliteResultSet
{
    NUT_REF_COUNTABLE

    rc_ptr<SqliteStatement> _stmt;

    int get_column_index(const char *column_name)
    {
        assert(nullptr != column_name && nullptr != _stmt);
        const int c = ::sqlite3_column_count(_stmt->get_raw_stmt());
        for (int i = 0; i < c; ++i)
        {
            const char *n = ::sqlite3_column_name(_stmt->get_raw_stmt(), i);
            assert(nullptr != n);
            if (0 == stricmp(n, column_name))
                return i;
        }
        return -1;
    }

public:
    SqliteResultSet() = default;

    SqliteResultSet(SqliteStatement *stmt)
        : _stmt(stmt)
    {
        assert(nullptr != stmt);
    }

    bool next()
    {
        if (nullptr == _stmt)
            return false;

        return SQLITE_ROW == ::sqlite3_step(_stmt->get_raw_stmt());
    }

    /**
     * @param pos is 0-based
     */
    int get_int(int column_index)
    {
        assert(column_index >= 0 && nullptr != _stmt);
        return ::sqlite3_column_int(_stmt->get_raw_stmt(), column_index);
    }

    int get_int(const char *column_name)
    {
        assert(nullptr != column_name && nullptr != _stmt);
        const int i = get_column_index(column_name);
        assert(i >= 0);
        return get_int(i);
    }

    /**
     * @param pos is 0-based
     */
    std::string get_string(int column_index)
    {
        assert(column_index >= 0 && nullptr != _stmt);
        const char *ret = (const char*) ::sqlite3_column_text(_stmt->get_raw_stmt(), column_index);
        return nullptr == ret ? "" : ret;
    }

    std::string get_string(const char *column_name)
    {
        assert(nullptr != column_name && nullptr != _stmt);
        const int i = get_column_index(column_name);
        assert(i >= 0);
        return get_string(i);
    }
};

}

#endif
