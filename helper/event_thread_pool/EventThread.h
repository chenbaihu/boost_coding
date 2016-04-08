#ifndef _UTIL_EVENT_THREAD_H_
#define _UTIL_EVENT_THREAD_H_

#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>

#include <vector>
#include <tr1/functional>
#include <tr1/memory>

struct event_base;

namespace util {
class PipedEventWatcher; 

typedef std::tr1::function<void()> Task;
typedef std::tr1::shared_ptr<Task> TaskPtr;

class EventThread { 
public:
    EventThread(bool detach = false);
    ~EventThread();

public:
    bool Init(int queue_size = 100000);
    bool Start();
    void Stop();
    void Close();

public:
    bool AddTask(TaskPtr& task);

public:
    struct event_base *event_base() { return event_base_; }

private:
    static void* Main(void *arg);
    void HandleStop();
    void HandleTask();

public:
    int queue_size_;
    struct event_base *event_base_;
    std::tr1::shared_ptr<PipedEventWatcher>  task_watcher_;

    volatile bool running_;
    std::tr1::shared_ptr<PipedEventWatcher>  stop_watcher_;

    std::vector<std::tr1::shared_ptr<Task> > tasks_;

    bool detach_;
    pthread_mutex_t mutex;
    pthread_t tid;
};
typedef std::tr1::shared_ptr<EventThread> EventThreadPtr;

} // namespace util

#endif // _UTIL_EVENT_THREAD_H_
