/**
 * @file -
 * @author jingqi
 * @date 2012-08-10
 * @last-edit 2014-11-21 22:55:43 jingqi
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

    int getColumnIndex(const char *columnName)
    {
        assert(NULL != columnName && !m_stmt.isNull());
        int c = ::sqlite3_column_count(m_stmt->raw());
        for (int i = 0; i < c; ++i)
        {
            const char *n = ::sqlite3_column_name(m_stmt->raw(), i);
            assert(NULL != n);
            if (strieq(n, columnName))
                return i;
        }
        return -1;
    }

public:
    ResultSet() {}

    ResultSet(ref<SqliteStmt> stmt)
        : m_stmt(stmt)
    {
        assert(!stmt.isNull());
    }

    bool next()
    {
        if (m_stmt.isNull())
            return false;

        int rs = ::sqlite3_step(m_stmt->raw());
        return SQLITE_ROW == rs;
    }

    /**
     * @param pos is 0-based
     */
    int getInt(int columnIndex)
    {
        assert(columnIndex >= 0 && !m_stmt.isNull());
        return ::sqlite3_column_int(m_stmt->raw(), columnIndex);
    }

    int getInt(const char *columnName)
    {
        assert(NULL != columnName && !m_stmt.isNull());
        int i = getColumnIndex(columnName);
        assert(i >= 0);
        return getInt(i);
    }

    /**
     * @param pos is 0-based
     */
    std::string getString(int columnIndex)
    {
        assert(columnIndex >= 0 && !m_stmt.isNull());
        const char *ret = (const char*) ::sqlite3_column_text(m_stmt->raw(), columnIndex);
        return NULL == ret ? "" : ret;
    }

    std::string getString(const char *columnName)
    {
        assert(NULL != columnName && !m_stmt.isNull());
        int i = getColumnIndex(columnName);
        assert(i >= 0);
        return getString(i);
    }
};

}

#endif
