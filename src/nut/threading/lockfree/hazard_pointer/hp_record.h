
#ifndef ___HEADFILE_135689F0_42A0_4544_AF2A_DCCFB31441F5_
#define ___HEADFILE_135689F0_42A0_4544_AF2A_DCCFB31441F5_

#include <atomic>


namespace nut
{

/**
 * Hazard-Pointer algothrim
 *
 * See https://blog.csdn.net/pongba/article/details/589864
 *
 * 假设存在以下竞争：
 * Thread A (使用 M):                            Thread B (删除 M):
 *  A1. Acquire HP                                B1. 从外部数据结构中删除对 M 的引用
 *    A1-1. 读 global_version                     B2. 设置 M 的 retire_mark
 *    A1-2. 添加/修改 version 到 HP_list          B3. 添加 M 到 retire_list
 *  A2. 检查 M 的 retire_mark                       B3-1. 读 global_version, 同时 ++global_version
 *  A3. 根据检查结果使用(放弃使用）这块内存         B3-2. 添加 version 和 M 到 retire_list
 *  A4. Release HP                                B4. 扫瞄 HP_list, retire_list
 *                                                  B4-1. 读取 HP_list, retire_list 的 version 信息
 *                                                  B4-2. 根据 version 信息，删除 M
 *
 * 由于 A2 和 B2 操作存在因果关系，配合内存栅栏，从而保证正常工作
 */
class HPRecord
{
public:
    static HPRecord* acquire();
    static void release(HPRecord *rec);

    /**
     * release then acquire again
     */
    void reacquire();

    /**
     * 清理资源
     *
     * NOTE 应该放到进程的最后来清理
     */
    static void clear();

private:
    HPRecord(size_t v);
    ~HPRecord() = default;

    HPRecord(const HPRecord&) = delete;
    HPRecord& operator=(const HPRecord&) = delete;

private:
    static std::atomic<HPRecord*> _head;
    static std::atomic<size_t> _list_size;

    std::atomic<size_t> _version = ATOMIC_VAR_INIT(0);
    std::atomic<bool> _valid = ATOMIC_VAR_INIT(true);
    HPRecord *_next = nullptr;

    friend class HPRetireList;
};

class HPGuard
{
public:
    /**
     * @param rec if nullptr passed, will automatically require one
     */
    HPGuard(HPRecord *rec = nullptr);
    ~HPGuard();

    void reacquire();

private:
    HPGuard(const HPGuard&) = delete;
    HPGuard& operator=(const HPGuard&) = delete;

private:
    HPRecord *_record;
};

}

#endif
