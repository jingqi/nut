/**
 * @file -
 * @author jingqi
 * @date 2011-11-19
 */

#ifndef ___HEADFILE_4AB18864_614E_4752_A996_3CC927369C09_
#define ___HEADFILE_4AB18864_614E_4752_A996_3CC927369C09_

#include <nut/memtool/refarg.hpp>
#include <nut/debugging/destroy_checker.hpp>
#include <nut/threading/lockfree/atomic.hpp>
#include <nut/mem/memory_allocator.hpp>

#include "enref.hpp"

namespace nut
{



}

/** 即使用户类的构造函数是private的，也能用gc_new */
#define NUT_GC_PRIVATE_GCNEW template <typename ___T> friend class nut::GCWrapper;

#endif
