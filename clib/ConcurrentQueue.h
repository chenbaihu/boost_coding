#ifndef _CLIB_CONCURRENTQUEUE_H_
#define _CLIB_CONCURRENTQUEUE_H_

#include <pthread.h>
#include <queue>

#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>

namespace clib {

template<typename Data>
class ConcurrentQueue {
private:
    std::queue<Data> the_queue;
    pthread_mutex_t  mutex;
    pthread_cond_t   condition;

public:
    ConcurrentQueue()
    {
        this->mutex = PTHREAD_MUTEX_INITIALIZER;
        this->condition = PTHREAD_COND_INITIALIZER;
    }
    ~ConcurrentQueue(){
    }

    void push(Data& data)
    {
        //TODO queue length limit
        pthread_mutex_lock(&this->mutex); 
        this->the_queue.push(data);
        pthread_mutex_unlock(&this->mutex);
        pthread_cond_signal(&this->condition);
    }

    bool empty()
    {
        pthread_mutex_lock(&this->mutex); 
        bool is_empty = this->the_queue.empty();
        pthread_mutex_unlock(&this->mutex);
        return is_empty;
    }

    size_t size()
    {
        pthread_mutex_lock(&this->mutex); 
        size_t size = this->the_queue.size();
        pthread_mutex_unlock(&this->mutex);
        return size;
    }

    bool try_pop(Data& popped_value)
    {
        pthread_mutex_lock(&this->mutex); 
        if(this->the_queue.empty())
        {
            pthread_mutex_unlock(&this->mutex);
            return false;
        }
        
        popped_value = this->the_queue.front();
        this->the_queue.pop();
        pthread_mutex_unlock(&this->mutex);
        return true;
    }

    void wait_and_pop(Data& popped_value)
    {
        pthread_mutex_lock(&this->mutex); 
        while (this->the_queue.empty()) {
            //pthread_cond_wait(&this->condition, &this->mutex);
            
            struct timeval cur_time; 
            gettimeofday(&cur_time, NULL);

            struct timespec wait_time; 
            wait_time.tv_sec  = cur_time.tv_sec;
            wait_time.tv_nsec = (cur_time.tv_usec + 20*1000/*20ms*/)*1000;

            pthread_cond_timedwait(&this->condition, &this->mutex, &wait_time);
            break;
        }
        popped_value = NULL;
        if (!this->the_queue.empty()) {
            popped_value = this->the_queue.front();
            this->the_queue.pop();
        }
        pthread_mutex_unlock(&this->mutex);
    } 
};

} //namespace clib 

#endif //_CLIB_CONCURRENTQUEUE_H_
