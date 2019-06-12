
#ifndef ___HEADFILE_D9A34E39_AC0F_446F_AEAA_B316EFC64AEA_
#define ___HEADFILE_D9A34E39_AC0F_446F_AEAA_B316EFC64AEA_

#include "../../nut_config.h"
#include "../../rc/rc_ptr.h"
#include "../log_record.h"
#include "../log_filter.h"


namespace nut
{

class NUT_API LogHandler
{
    NUT_REF_COUNTABLE

public:
    void set_flush_mask(loglevel_mask_type mask) noexcept
    {
        _flush_mask = mask;
    }

    LogFilter& get_filter() noexcept
    {
        return _filter;
    }

    virtual void handle_log(const LogRecord& rec) noexcept = 0;

protected:
    // 控制哪些日志触发立即刷新到磁盘
    loglevel_mask_type _flush_mask = LL_FATAL;

private:
    // 日志过滤器
    LogFilter _filter;
};

}

#endif
