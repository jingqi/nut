/**
 * @file -
 * @author jingqi
 * @date 
 * @last-edit 2012-08-11 16:25:08 jingqi
 */

#ifndef ___HEADFILE_54107F5A_983A_4021_AB0E_0E5422FE5FD9_
#define ___HEADFILE_54107F5A_983A_4021_AB0E_0E5422FE5FD9_

#include <vector>
#include <string>
#include <sqlite3.h>

#include <nut/gc/gc.hpp>
#include <nut/gc/enref.hpp>

#include "wrapers.hpp"

namespace nut
{

class ParamWraper
{
public:
    enum ParamType
    {
        NONE,
        INTEGER,
        STRING
    };

    ParamType type;
    ref<enref<std::string> > string_arg;
    int int_arg;

private:
    ParamWraper() : type(NONE) {}

public:
    ParamWraper(int arg)
        : type(INTEGER), int_arg(arg)
    {}

    ParamWraper(const char *arg)
        : type(STRING), string_arg(gc_new<enref<std::string> >(arg))
    {}

    ParamWraper(const std::string& arg)
        : type(STRING), string_arg(gc_new<enref<std::string> >(const_ref_arg<std::string>(arg)))
    {}

    static const ParamWraper& none()
    {
        static ParamWraper ret;
        return ret;
    }
};


class PreparedStatement
{
    NUT_GC_REFERABLE

    ref<SqliteStmt> m_stmt;
    std::vector<ref<enref<std::string> > > m_strings;

public:
    PreparedStatement() {}

    PreparedStatement(sqlite3 *db, const char *sql)
    {
        assert(NULL != db && NULL != sql);
        prepare(db, sql);
    }

    bool prepare(sqlite3 *db, const char *sql)
    {
        assert(NULL != db && NULL != sql);
        sqlite3_stmt *stmt;
        int rs = ::sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (SQLITE_OK != rs || NULL == stmt)
            return false;
        m_stmt = gc_new<SqliteStmt>(stmt);
        return true;
    }

    inline bool isValid() const
    {
        return !m_stmt.isNull();
    }

    ref<SqliteStmt> stmt()
    {
        return m_stmt;
    }

    bool reset()
    {
        assert(isValid());
        m_strings.clear();
        return SQLITE_OK == ::sqlite3_reset(m_stmt->raw());
    }
    
    /**
     * @param pos 1-based
     */
    bool bind(int pos, const ParamWraper& param)
    {
        assert(isValid());
        switch (param.type)
        {
        case ParamWraper::INTEGER:
            return SQLITE_OK == ::sqlite3_bind_int(m_stmt->raw(), pos, param.int_arg);

        case ParamWraper::STRING:
            // sqlite3 只有在用到字符串时才会去取值，这里需要缓存一下字符串
            assert(!param.string_arg.isNull());
            m_strings.push_back(param.string_arg);
            return SQLITE_OK == ::sqlite3_bind_text(m_stmt->raw(), pos, param.string_arg->c_str(), -1, NULL);

        default:
            return true;
        }
    }
};

}

#endif

