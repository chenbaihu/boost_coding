#include "EventWatcher.h"

#include <assert.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "Log.h"

namespace clib {

EventWatcher::EventWatcher(struct event_base *event_base, 
        const Handler& handler)
    : event_base_(event_base)
    , handler_(handler)
{
    assert(event_base_);
}

EventWatcher::~EventWatcher() 
{
    Cancel();
}

bool EventWatcher::Init() {
    if (!DoInit()) {
        return false;
    }
    event_base_set(event_base_, &event_);
    return true;
}

bool EventWatcher::Watch(const struct timeval *timeout) 
{
    if (event_add(&event_, timeout) != 0) {
        return false;
    }
    return true;
}

void EventWatcher::Cancel() 
{
    if (event_initialized(&event_)) {
        event_del(&event_);
    }
    memset(&event_, 0, sizeof(event_));
}

void EventWatcher::Close() 
{
    Cancel();
    DoClose();
}

// PipedEventWatcher
PipedEventWatcher::PipedEventWatcher(struct event_base *event_base,
                                     const Handler& handler)
    : EventWatcher(event_base, handler)
{
    memset(pipe_, 0, sizeof(pipe_[0] * 2));
}

bool PipedEventWatcher::DoInit() 
{
    assert(pipe_[0] == 0);
    if (evutil_socketpair(AF_LOCAL, SOCK_STREAM, 0, pipe_) < 0) {
        goto failed;
    }

    if (evutil_make_socket_nonblocking(pipe_[0]) < 0
        || evutil_make_socket_nonblocking(pipe_[1]) < 0) {
        goto failed;
    }

    event_assign(&event_, 
                 event_base_, 
                 pipe_[1],
                 EV_READ | EV_PERSIST,
                 PipedEventWatcher::HandlerFn,
                 this);
    return true;
failed:
    Close();
    return false;
}

void PipedEventWatcher::DoClose() 
{
    if (pipe_[0]) {
        close(pipe_[0]);
        close(pipe_[1]);
        memset(pipe_, 0, sizeof(pipe_[0]) * 2);
    }
}

void PipedEventWatcher::HandlerFn(int fd, short which, void *v) 
{
    PipedEventWatcher* h = reinterpret_cast<PipedEventWatcher*>(v);

    char buf[128];
    ssize_t n = 0;
    if ((n = recv(h->pipe_[1], buf, sizeof(buf), 0)) < 0) {
        log_error("Recv notify failed: %s", strerror(errno));
    } else if (n == 0) {
        log_error("Notify fd closed.");
    } else {
        h->handler_();
    }
}

void PipedEventWatcher::Notify() 
{
    char buf[1] = {};
    if (send(pipe_[0], buf, sizeof(buf), 0) < 0) {
        return;
    }
}

// TimedEventWatcher
TimedEventWatcher::TimedEventWatcher(struct event_base *event_base,
                                     const Handler& handler)
    : EventWatcher(event_base, handler)
{
}

bool TimedEventWatcher::DoInit() 
{
    evtimer_assign(&event_, event_base_, TimedEventWatcher::HandlerFn, this);
    return true;
}

void TimedEventWatcher::HandlerFn(int fd, short which, void *v) 
{
    TimedEventWatcher* h = reinterpret_cast<TimedEventWatcher*>(v);
    h->handler_();
}

// SignalEventWatcher
SignalEventWatcher::SignalEventWatcher(int signo,
                                       struct event_base *event_base,
                                       const Handler& handler)
    : EventWatcher(event_base, handler)
    , signo_(signo)
{
    assert(signo_);
}

bool SignalEventWatcher::DoInit()
{
    assert(signo_);
    evsignal_assign(&event_, event_base_, signo_, SignalEventWatcher::HandlerFn, this);
    return true;
}

void SignalEventWatcher::HandlerFn(int sn, short which, void *v) 
{
    SignalEventWatcher* h = reinterpret_cast<SignalEventWatcher*>(v);
    h->handler_();
}

} // clib 
