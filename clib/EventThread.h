#ifndef _CLIB_EVENT_THREAD_H_
#define _CLIB_EVENT_THREAD_H_

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>
//#include <tr1/functional>
#include <event.h>

namespace clib {

class PipedEventWatcher;

class EventThread {
public:
    //typedef std::tr1::function<void()> Task;
    typedef boost::function<void()> Task;
    typedef boost::shared_ptr<Task>    TaskPtr;

public:
    EventThread(bool detach = false);
    ~EventThread();

    bool Init(int queue_size = 0);
    bool Start();
    void Stop();
    void Close();

    boost::thread::id id() const;

    bool AddTask(TaskPtr task);

    struct event_base *event_base() { return event_base_; }

private:
    void Main();
    void HandleStop();
    void HandleTask();

    int queue_size_;
    struct event_base *event_base_;
    boost::scoped_ptr<PipedEventWatcher> task_watcher_;

    volatile bool running_;
    boost::scoped_ptr<PipedEventWatcher> stop_watcher_;

    std::vector<boost::shared_ptr<Task> > tasks_;

    bool detach_;
    boost::mutex tasks_mutex_;
    boost::shared_ptr<boost::thread> thread_;
};

typedef boost::shared_ptr<EventThread> EventThreadPtr;
} // namespace clib

#endif // _CLIB_EVENT_THREAD_H_
