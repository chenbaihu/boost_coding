#include "mutex.h"
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include "macro_define.h"
#include "log.h"


struct MutexImpl
{
    pthread_mutex_t mutex;  
};

Mutex::Mutex()
    :mutex_(new MutexImpl)
{
    int ret = ::pthread_mutex_init(&(mutex_->mutex), 0);
    if(0 != ret)
    {
        assert(ENOMEM == ret);
        log_error("error pthread_mutex_init ret=",ret);
    }
}

Mutex::~Mutex()
{
    ::pthread_mutex_destroy(&(mutex_->mutex));
}

void Mutex::lock()
{
    int ret = ::pthread_mutex_lock(&(mutex_->mutex));
    if(0 != ret)
    {
        assert(EDEADLK == ret);
        log_error("error lock ret=",ret);
    }
}

void Mutex::unlock()
{
    int ret = ::pthread_mutex_unlock(&(mutex_->mutex));
    if(0 != ret)
    {
        assert(EPERM == ret);
        log_error("error unlock ret=",ret);
    }
}

void * Mutex::impl()
{
    return &(mutex_->mutex);
}


