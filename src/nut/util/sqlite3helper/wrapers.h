
#ifndef ___HEADFILE_F597ABF0_3C94_490D_A072_5EA0331BFD83_
#define ___HEADFILE_F597ABF0_3C94_490D_A072_5EA0331BFD83_

#include <sqlite3.h>

#include <nut/rc/rc_new.h>

namespace nut
{

class SqliteStmt
{
    NUT_REF_COUNTABLE

    sqlite3_stmt *_stmt = NULL;

public:
    SqliteStmt(sqlite3_stmt *stmt)
        : _stmt(stmt)
    {
        assert(NULL != stmt);
    }

    ~SqliteStmt()
    {
        if (NULL != _stmt)
        {
            int rs = ::sqlite3_finalize(_stmt);
            assert(SQLITE_OK == rs);
            (void)rs;
            _stmt = NULL;
        }
    }

    sqlite3_stmt* raw()
    {
        return _stmt;
    }
};

}

#endif
