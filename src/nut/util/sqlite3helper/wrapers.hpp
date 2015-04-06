
#ifndef ___HEADFILE_F597ABF0_3C94_490D_A072_5EA0331BFD83_
#define ___HEADFILE_F597ABF0_3C94_490D_A072_5EA0331BFD83_

#include <sqlite3.h>

#include <nut/rc/rc_new.hpp>

namespace nut
{

class SqliteStmt
{
    NUT_REF_COUNTABLE

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


