/**
 * @file -
 * @author jingqi
 * @date 2012-08-11
 * @last-edit 2012-08-11 16:44:24 jingqi
 */

#ifndef ___HEADFILE_F597ABF0_3C94_490D_A072_5EA0331BFD83_
#define ___HEADFILE_F597ABF0_3C94_490D_A072_5EA0331BFD83_

#include <sqlite3.h>

#include <nut/gc/gc.hpp>

namespace nut
{

class SqliteStmt
{
    NUT_GC_REFERABLE

    sqlite3_stmt *m_stmt;

public:
    SqliteStmt(sqlite3_stmt *stmt)
        : m_stmt(stmt)
    {
        assert(NULL != stmt);
    }

    ~SqliteStmt()
    {
        if (NULL != m_stmt)
        {
            int rs = ::sqlite3_finalize(m_stmt);
            assert(SQLITE_OK == rs);
            m_stmt = NULL;
        }
    }

    sqlite3_stmt* raw()
    {
        return m_stmt;
    }
};

}

#endif


