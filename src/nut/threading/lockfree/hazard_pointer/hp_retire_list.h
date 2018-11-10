
#ifndef ___HEADFILE_DEA56935_FD6E_412D_A149_AAD65CF85A18_
#define ___HEADFILE_DEA56935_FD6E_412D_A149_AAD65CF85A18_

#include <stdlib.h> // for ::free()
#include <list>
#include <atomic>


namespace nut
{

/**
 * NOTE Instances of this class should only be THREAD-LOCAL
 */
class HPRetireList
{
public:
    typedef void (*retire_func_type)(void*);

private:
    class RetireRecord
    {
    public:
        RetireRecord(retire_func_type rfunc, void *ud, size_t v)
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
    static void retire_object(T *obj)
    {
        retire_any(delete_object<T>, obj);
    }

    /**
     * memory will be freed
     */
    static void retire_memory(void *ptr);

    /**
     * any resource will be retired (freed or released)
     */
    static void retire_any(retire_func_type rfunc, void *userdata = nullptr);

private:
    HPRetireList() = default;
    ~HPRetireList();

    HPRetireList(const HPRetireList&) = delete;
    HPRetireList& operator=(const HPRetireList&) = delete;

    template <typename T>
    static void delete_object(void *p)
    {
        ((T*) p)->~T();
        ::free(p);
    }

    static void scan_retire_list(HPRetireList *rl);

private:
    static std::atomic<size_t> _global_version;

    std::list<RetireRecord> _retire_list;

    friend class HPRecord;
};

}

#endif
