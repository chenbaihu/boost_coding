#ifndef __THREAD_LOCK_H
#define __THREAD_LOCK_H

#include <stdio.h>
#include <pthread.h>

namespace util {

inline void TsAddMs(struct timespec *ts, long ms) 
{
	int sec = ms/1000;
	ms = ms-sec*1000;

	ts->tv_nsec += ms*1000000;
	ts->tv_sec  += ts->tv_nsec/1000000000 + sec;
	ts->tv_nsec  = ts->tv_nsec%1000000000;
}

class Mutex {
protected:
    pthread_mutex_t lock_;

public:
    Mutex(pthread_mutexattr_t* attrs = 0) {        
        pthread_mutex_init(&lock_,attrs);
    }

    ~Mutex() {
        pthread_mutex_destroy(&lock_);
    }

    int lock() {
        return pthread_mutex_lock(&lock_);
    }

    int lock_t(const struct timespec* time) {
        return pthread_mutex_timedlock(&lock_, time);
    }

    int unlock() {
        return  pthread_mutex_unlock(&lock_);
    }

    pthread_mutex_t& get_lock() {
        return lock_;
    }
};

class RWLock {
private:
	pthread_rwlock_t lock_;		

public:
	explicit RWLock(const pthread_rwlockattr_t* attr = NULL) {
		::pthread_rwlock_init(&lock_, attr);
	} 

	~RWLock() { 
		::pthread_rwlock_destroy(&lock_);
	}

	int rlock() {
		return pthread_rwlock_rdlock(&lock_);
	}

	int wlock() {
		return pthread_rwlock_wrlock(&lock_);
	}

	int rlock_t(uint64_t time_out_ms=0) {
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME,&ts);
		TsAddMs(&ts,time_out_ms);
		return pthread_rwlock_timedrdlock(&lock_, &ts);
	}

	int wlock_t(uint64_t time_out_ms=0) {
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME,&ts);
		TsAddMs(&ts,time_out_ms);
		return pthread_rwlock_timedwrlock(&lock_,&ts);
	}

	int ulock() {
		return pthread_rwlock_unlock(&lock_);
	}
};

class NullMutex {
public:
    NullMutex(pthread_mutexattr_t* attrs = 0)
        : lock_ (0) 
    {
    }

    ~NullMutex() {}

    int lock() {
        return 0;
    }

    int lock_t(const struct timespec* time) {
        errno = ETIME; return -1;
    }

    int unlock() {
        return 0;
    }

    int& get_lock() {
        return lock_;
    }

private:
    int lock_; 
};

template <class MyLOCK>
class Guard {
private:
    MyLOCK* lock_;
    int owner_;

public:
    Guard(MyLOCK& mylock):lock_(&mylock) {        
        lock();
    }

    ~Guard() {
        unlock();
    }    

    int lock() {
        return owner_ = lock_->lock();
    }

    int unlock() {
        if (owner_ == -1) {
          return -1;
        } else {
            owner_ = -1;
            return lock_->unlock();
        }
    }

    int locked() const {
        return owner_ != -1;
    }

private:
    Guard();
    Guard (const Guard<MyLOCK> &);
    void operator= (const Guard<MyLOCK> &);
};

template<>
class Guard<NullMutex> {
public:
    Guard(NullMutex& lock){}
    ~Guard(){}    

    int lock() {
        return 0;
    }

    int unlock() {
        return 0;
    }

    int locked() const {
        return 1;
    }

private:
    Guard();
    Guard (const Guard<NullMutex> &);
    void operator= (const Guard<NullMutex> &);    
};

template <class MUTEX>
class Condition {
public:
    Condition (MUTEX &m):mutex_(m) {
        pthread_condattr_t attr;
        pthread_condattr_init(&attr);

        if (pthread_cond_init(&this->cond_, &attr) != 0) {
            printf("pthread_cond_init failed\n");
        }
        pthread_condattr_destroy(&attr);
    }

    ~Condition (void) {
        if (this->remove () == -1) {
            printf("remove condition failed\n");
        }
    }

    int wait (void) {
        return pthread_cond_wait(&this->cond_, &this->mutex_.get_lock());
    }

    int wait (const struct timespec* time) {
        return pthread_cond_timedwait(&this->cond_, &this->mutex_.get_lock(), time);
    }

    int signal (void) {
        return pthread_cond_signal(&this->cond_);
    }

    int broadcast (void) {
        return pthread_cond_broadcast(&this->cond_);
    }

    int remove (void) {
        int result = 0;

        while ((result = pthread_cond_destroy(&this->cond_)) == -1 && errno == EBUSY) {
            pthread_cond_broadcast(&this->cond_);
        }
        return result;
    }

    MUTEX &mutex (void) {
        return this->mutex_;
    }

protected:
    pthread_cond_t cond_;
    MUTEX &mutex_;

private:
    void operator= (const Condition<MUTEX> &);
    Condition (const Condition<MUTEX> &);
};

} // end of namespace util

#endif


