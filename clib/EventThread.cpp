#include "EventThread.h"
#include "EventWatcher.h"

#include "Log.h"

namespace clib {

using namespace std;
using namespace boost;

const int kDefaultQueueSize = 1024;

EventThread::EventThread(bool detach)
    : queue_size_(kDefaultQueueSize)
    , event_base_(NULL)
    , running_(false)
    , detach_(detach)
{
}

EventThread::~EventThread() {
    //Stop();
    //Close();
    
    if (task_watcher_.get() != NULL) {
        task_watcher_->Close();
    }

    task_watcher_.reset();
    stop_watcher_.reset();
}

bool EventThread::Init(int queue_size) 
{
    assert(!event_base_);
    assert(!running_);

    if (queue_size > 0) {
        queue_size_ = queue_size;
    }

    struct event_config* cfg = event_config_new();
    event_config_set_flag(cfg, EVENT_BASE_FLAG_NO_CACHE_TIME);
    event_base_ = event_base_new_with_config(cfg);
    event_config_free(cfg);

    if (!event_base_) {
        return false;
    }
    
    task_watcher_.reset(new PipedEventWatcher(event_base_, 
                                              std::tr1::bind(&EventThread::HandleTask, this)));
    if (!task_watcher_->Init() || !task_watcher_->Watch()) {
        Close();
        return false;
    }
    return true;
}

bool EventThread::Start() 
{
    assert(event_base_);
    assert(!running_);

    stop_watcher_.reset(new PipedEventWatcher(event_base_,
                                              std::tr1::bind(&EventThread::HandleStop, this)));
    if (!stop_watcher_->Init() || !stop_watcher_->Watch()) {
        return false;
    }

    thread_.reset(new boost::thread(std::tr1::bind(&EventThread::Main, this)));
    if (detach_) {
        thread_->detach();
    }

    running_ = true;
    return true;
}

boost::thread::id EventThread::id() const 
{
    assert(event_base_);
    assert(running_);
    return thread_->get_id();
}

void EventThread::Stop() 
{
    if (running_) {
        running_ = false;
        stop_watcher_->Notify();
        thread_->join();
    }
}

void EventThread::Close() 
{
    if (event_base_) {
        event_base_free(event_base_);
        event_base_ = NULL;
    }
}

void EventThread::HandleStop() 
{
    while (!tasks_.empty())
    {
        HandleTask();
    }
    event_base_loopbreak(event_base_);
}

bool EventThread::AddTask(TaskPtr task) 
{
    bool need_notify = false;
    
    {
        boost::mutex::scoped_lock lock(tasks_mutex_);
        if (tasks_.empty()) 
        {
            need_notify = true;
        }
        else if (tasks_.size() >= (size_t)queue_size_) 
        {
            return false;
        }
        tasks_.push_back(task);
    }

    if (need_notify) {
        task_watcher_->Notify();
    }
    return true;
}

void EventThread::HandleTask() 
{
    std::vector<TaskPtr> tasks;

    {
        boost::mutex::scoped_lock lock(tasks_mutex_);
        tasks.swap(tasks_);
    }

    size_t task_size = tasks.size();
    for (size_t i=0; i<task_size; i++) {
        (*(tasks[i]))();
    }
}

void EventThread::Main() 
{
    event_base_loop(event_base_, 0);
}

} // namespace clib 
