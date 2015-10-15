#ifndef _CLIB_EVENT_THREAD_POOL_H_
#define _CLIB_EVENT_THREAD_POOL_H_

#include "EventThread.h"

#include <tr1/functional>
#include <tr1/memory>
#include <vector>

namespace clib {

class EventThreadPool {
public:
    EventThreadPool(uint16_t _numThreads=5);
    ~EventThreadPool();

public:
    bool Start();
    void Stop();
    EventThreadPtr GetNextEventThread();

private:
    uint16_t numThreads;
    bool     started;
    uint64_t next;
    std::vector<EventThreadPtr> eventThreadPtrVec;
};
typedef std::tr1::shared_ptr<EventThreadPool> EventThreadPoolPtr;

} // namespace clib

#endif //_CLIB_EVENT_THREAD_POOL_H_
