
#ifndef ___HEADFILE_DEA56935_FD6E_412D_A149_AAD65CF85A18_
#define ___HEADFILE_DEA56935_FD6E_412D_A149_AAD65CF85A18_

#include <stdlib.h> // for ::free()
#include <list>
#include <atomic>

#include "../../../nut_config.h"
#include "../../../debugging/destroy_checker.h"


namespace nut
{

/**
 * NOTE Instances of this class should only be THREAD-LOCAL
 */
class NUT_API HPRetireList
{
    friend class HPRecord;

public:
    typedef void (*retire_func_type)(void*);

private:
    class RetireRecord
    {
    public:
        RetireRecord(retire_func_type rfunc, void *ud, size_t v) noexcept
            : retire_func(rfunc), userdata(ud), version(v)
        {}

        RetireRecord(const RetireRecord& x) = default;

    public:
        retire_func_type const retire_func;
        void *const userdata;
        const size_t version;
    };

public:
    /**
     * object will be destructed and memory be freed
     */
    template <typename T>
    static void retire_object(T *obj) noexcept
    {
        retire_any(delete_object<T>, obj);
    }

    /**
     * memory will be freed
     */
    static void retire_memory(void *ptr) noexcept;

    /**
     * any resource will be retired (freed or released)
     */
    static void retire_any(retire_func_type rfunc, void *userdata = nullptr) noexcept;

private:
    HPRetireList() = default;
    ~HPRetireList() noexcept;

    HPRetireList(const HPRetireList&) = delete;
    HPRetireList& operator=(const HPRetireList&) = delete;

    template <typename T>
    static void delete_object(void *p) noexcept
    {
        ((T*) p)->~T();
        ::free(p);
    }

    static void scan_retire_list(HPRetireList *rl) noexcept;

private:
    NUT_DEBUGGING_DESTROY_CHECKER

    static std::atomic<size_t> _global_version;

    std::list<RetireRecord> _retire_list;
};

}

#endif
