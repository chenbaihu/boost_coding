#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

#include <string>
#include <iostream>
#include <deque>
#include <stdio.h>

class ThreadPool : private boost::noncopyable {
public:
    typedef boost::function<void ()> Task;
    typedef boost::mutex::scoped_lock ScopedLock;

public:
    ThreadPool();
    ~ThreadPool();

public:
    void Start(int num_threads);
    void Stop();

    void Run(const Task& task);

public:
    void set_max_queue_size(size_t max_queue_size=20000) { max_queue_size_ = max_queue_size; }

private:
    void RunInThread(); 
    bool IsFull();
    Task GetTake();

private:
    bool run_flag_;
    boost::scoped_ptr<boost::thread_group> thread_pool_;
    int num_threads_;
    boost::mutex     mutex_;
    boost::condition cond_;
    std::deque<Task> queue_;
    size_t max_queue_size_;
};

ThreadPool::ThreadPool() 
    : run_flag_(false), num_threads_(10), max_queue_size_(20000)
{
}

ThreadPool::~ThreadPool()
{
}

void ThreadPool::Start(int num_threads) {
    num_threads_ = num_threads;
    run_flag_ = true;
    thread_pool_.reset(new boost::thread_group);

    int i = 0;
    for (i=0; i<num_threads_; i++) {
        thread_pool_->create_thread(boost::bind(&ThreadPool::RunInThread, this));
    }
}

void ThreadPool::Stop() {
    run_flag_ = false;
    thread_pool_->join_all();
}

void ThreadPool::Run(const Task& task) {
    if (!run_flag_) { 
        task();
        return;
    }
    ScopedLock lock(mutex_);
    if (IsFull()) {
        cond_.wait(lock);
    }
    queue_.push_back(task); 
    cond_.notify_one();
}

bool ThreadPool::IsFull() {
    // must lock use
    if (max_queue_size_>0&&queue_.size()>=max_queue_size_) {
        return true;
    }
    return false;
}

void ThreadPool::RunInThread() {
    while (run_flag_) {  
        GetTake()();
    }
}

ThreadPool::Task ThreadPool::GetTake() {
    ScopedLock lock(mutex_);
    if (queue_.empty()) {
        cond_.wait(lock);
    }

    if (queue_.empty()) {
        return Task();
    }

    Task task = queue_.front(); 
    queue_.pop_front();
    cond_.notify_one();
    return task;
}

void test_thread_pool(int i)
{
    printf("%ld %d\n", pthread_self(), i);
}

int main(int argc, char* argv[])
{
    ThreadPool thread_pool;
    thread_pool.set_max_queue_size(10000);
    thread_pool.Start(10);
    int i = 0;
    for (i=0; i<100000; i++) {
        thread_pool.Run(boost::bind(test_thread_pool, i));
    }
    thread_pool.Stop();
    return 0;
}


