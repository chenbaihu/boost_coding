#include "EventThreadPool.h"

namespace clib {

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
        return false;
    }

    eventThreadPtrVec.resize(numThreads);
    for (uint16_t i=0; i<numThreads; i++) {
        EventThreadPtr eventThreadPtr(new EventThread);
        if (!eventThreadPtr->Init(20000)) {
            return false;
        }
        if (!eventThreadPtr->Start()) {
            return false;
        }
        eventThreadPtrVec[i] = eventThreadPtr; 
    }
    started = true;
    return true;
}

EventThreadPtr EventThreadPool::GetNextEventThread()
{
    if (numThreads==0 || eventThreadPtrVec.empty()) {
        return EventThreadPtr();
    }

    if (!started) {
        return EventThreadPtr();
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

} // namespace clib
