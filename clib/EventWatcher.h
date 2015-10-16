#ifndef _CLIB_EVENT_WATCHER_H_
#define _CLIB_EVENT_WATCHER_H_

#include <sys/time.h>
#include <tr1/functional>

#include <event.h>
#include <event_struct.h>

namespace clib {

class EventWatcher  
{
public:
    typedef std::tr1::function<void()> Handler;

    EventWatcher(struct event_base* event_base, const Handler& handler);
    virtual ~EventWatcher();

    bool Init();
    bool Watch(const struct timeval *timeout = NULL);
    void Cancel();
    void Close();

protected:
    virtual bool DoInit() = 0;
    virtual void DoClose() {}

    struct event_base *event_base_;
    struct event event_;
    Handler handler_;
};

class PipedEventWatcher : public EventWatcher
{
public:
    PipedEventWatcher(struct event_base* event_base,
                      const Handler& handler);
    void Notify();

private:
    virtual bool DoInit();
    virtual void DoClose();
    static void HandlerFn(int fd, short which, void *v);

    int pipe_[2];
};

class TimedEventWatcher : public EventWatcher 
{
public:
    TimedEventWatcher(struct event_base* event_base,
                      const Handler& handler);

private:
    virtual bool DoInit();
    static void HandlerFn(int fd, short which, void *v);
};

class SignalEventWatcher : public EventWatcher
{
public:
    SignalEventWatcher(int signo,
                       struct event_base* event_base,
                       const Handler& handler);

private:
    virtual bool DoInit();
    static void HandlerFn(int sn, short which, void *v);

    int signo_;
};

} // namespace clib

#endif // _CLIB_EVENT_WATCHER_H_
