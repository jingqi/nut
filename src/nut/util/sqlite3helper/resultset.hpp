/**
 * @file -
 * @author jingqi
 * @date 2012-08-10
 * @last-edit 2015-01-06 23:49:16 jingqi
 */

#ifndef ___HEADFILE_B51CBEE3_B5FB_4481_AE29_559B709E3B26_
#define ___HEADFILE_B51CBEE3_B5FB_4481_AE29_559B709E3B26_

#include <assert.h>
#include <string>
#include <string.h>
#include <sqlite3.h>

#include <nut/gc/gc.hpp>
#include <nut/util/string/string_util.hpp>

#include "wrapers.hpp"

namespace nut
{

class ResultSet
{
    NUT_GC_REFERABLE

    ref<SqliteStmt> m_stmt;

    int getColumnIndex(const char *column_name)
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

    ResultSet(ref<SqliteStmt> stmt)
        : m_stmt(stmt)
    {
        assert(!stmt.is_null());
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
    int get_int(int columnIndex)
    {
        assert(columnIndex >= 0 && !m_stmt.is_null());
        return ::sqlite3_column_int(m_stmt->raw(), columnIndex);
    }

    int get_int(const char *column_name)
    {
        assert(NULL != column_name && !m_stmt.is_null());
        int i = getColumnIndex(column_name);
        assert(i >= 0);
        return get_int(i);
    }

    /**
     * @param pos is 0-based
     */
    std::string get_string(int columnIndex)
    {
        assert(columnIndex >= 0 && !m_stmt.is_null());
        const char *ret = (const char*) ::sqlite3_column_text(m_stmt->raw(), columnIndex);
        return NULL == ret ? "" : ret;
    }

    std::string get_string(const char *column_name)
    {
        assert(NULL != column_name && !m_stmt.is_null());
        int i = getColumnIndex(column_name);
        assert(i >= 0);
        return get_string(i);
    }
};

}

#endif
