#ifndef _UTIL_EVENT_THREAD_POOL_H_
#define _UTIL_EVENT_THREAD_POOL_H_

#include <tr1/functional>
#include <tr1/memory>
#include <stdint.h>
#include <vector>

namespace util { 

typedef std::tr1::function<void()> Task;
typedef std::tr1::shared_ptr<Task> TaskPtr;

class EventThread;
typedef std::tr1::shared_ptr<EventThread> EventThreadPtr;

class EventThreadPool {
public:
    EventThreadPool(uint16_t _numThreads=5);
    ~EventThreadPool();

public:
    bool Start();
    void Stop();

public:
    bool AddTask(TaskPtr& task);

public:
    EventThreadPtr& GetNextEventThread();

private:
    uint16_t numThreads;
    bool     started;
    uint64_t next;
    std::vector<EventThreadPtr> eventThreadPtrVec;
};
typedef std::tr1::shared_ptr<EventThreadPool> EventThreadPoolPtr;

} // namespace util

#endif //_UTIL_EVENT_THREAD_POOL_H_
