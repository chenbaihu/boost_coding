#include "EventThreadPool.h"

#include "EventThread.h"
#include "log.h"

namespace util {

const static int g_max_task_in_queue = 10000;

EventThreadPool::EventThreadPool(uint16_t _numThreads/*=5*/)
    : numThreads(_numThreads),
      started(false),
      next(0)
{
}

EventThreadPool::~EventThreadPool()
{
    Stop();
}

bool EventThreadPool::Start()
{
    if (started) {
        return true;
    }

    if (numThreads<=0) {
        log_error("numThreads=%d", numThreads);
        return false;
    }

    eventThreadPtrVec.resize(numThreads);
    for (uint16_t i=0; i<numThreads; i++) {
        EventThreadPtr eventThreadPtr(new EventThread);
        if (!eventThreadPtr->Init(g_max_task_in_queue)) {
            log_error("eventThreadPtr->Init failed g_max_task_in_queue=%d", g_max_task_in_queue);
            return false;
        }
        if (!eventThreadPtr->Start()) {
            log_error("eventThreadPtr->Start failed");
            return false;
        }
        eventThreadPtrVec[i] = eventThreadPtr; 
    }
    started = true;
    return true;
}

bool EventThreadPool::AddTask(TaskPtr& task)
{
    EventThreadPtr& event_thread_ptr = GetNextEventThread();
    if (event_thread_ptr==NULL) {
        log_error("event_thread_ptr==NULL");
        return false;
    }
    return event_thread_ptr->AddTask(task);
}

EventThreadPtr& EventThreadPool::GetNextEventThread()
{
    static EventThreadPtr null_event_thread_ptr;
    if (numThreads==0 || eventThreadPtrVec.empty()) {
        return null_event_thread_ptr;
    }

    if (!started) {
        return null_event_thread_ptr;
    }

    uint64_t tnext = next;
    int index = tnext%eventThreadPtrVec.size();
    next++;
    return eventThreadPtrVec[index];
}

void EventThreadPool::Stop() {
    started = false;
    for (size_t i=0; i<eventThreadPtrVec.size(); i++) {
        EventThreadPtr& eventThreadPtr = eventThreadPtrVec[i];
        if (eventThreadPtr!=NULL) {
            eventThreadPtr->Stop();
            eventThreadPtr->Close();
        }
    }
    eventThreadPtrVec.clear();
}

} // namespace util
