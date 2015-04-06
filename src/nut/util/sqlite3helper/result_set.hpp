
#ifndef ___HEADFILE_B51CBEE3_B5FB_4481_AE29_559B709E3B26_
#define ___HEADFILE_B51CBEE3_B5FB_4481_AE29_559B709E3B26_

#include <assert.h>
#include <string>
#include <string.h>
#include <sqlite3.h>

#include <nut/rc/rc_new.hpp>
#include <nut/util/string/string_util.h>

#include "wrapers.hpp"

namespace nut
{

class ResultSet
{
    NUT_REF_COUNTABLE

    rc_ptr<SqliteStmt> m_stmt;

    int get_column_index(const char *column_name)
    {
        assert(NULL != column_name && !m_stmt.is_null());
        int c = ::sqlite3_column_count(m_stmt->raw());
        for (int i = 0; i < c; ++i)
        {
            const char *n = ::sqlite3_column_name(m_stmt->raw(), i);
            assert(NULL != n);
            if (strieq(n, column_name))
                return i;
        }
        return -1;
    }

public:
    ResultSet() {}

    ResultSet(rc_ptr<SqliteStmt> stmt)
        : m_stmt(stmt)
    {
        assert(stmt.is_not_null());
    }

    bool next()
    {
        if (m_stmt.is_null())
            return false;

        int rs = ::sqlite3_step(m_stmt->raw());
        return SQLITE_ROW == rs;
    }

    /**
     * @param pos is 0-based
     */
    int get_int(int column_index)
    {
        assert(column_index >= 0 && !m_stmt.is_null());
        return ::sqlite3_column_int(m_stmt->raw(), column_index);
    }

    int get_int(const char *column_name)
    {
        assert(NULL != column_name && !m_stmt.is_null());
        int i = get_column_index(column_name);
        assert(i >= 0);
        return get_int(i);
    }

    /**
     * @param pos is 0-based
     */
    std::string get_string(int column_index)
    {
        assert(column-index >= 0 && !m_stmt.is_null());
        const char *ret = (const char*) ::sqlite3_column_text(m_stmt->raw(), column_index);
        return NULL == ret ? "" : ret;
    }

    std::string get_string(const char *column_name)
    {
        assert(NULL != column_name && !m_stmt.is_null());
        int i = get_column_index(column_name);
        assert(i >= 0);
        return get_string(i);
    }
};

}

#endif
