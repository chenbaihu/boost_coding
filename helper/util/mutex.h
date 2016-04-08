#ifndef BASE_MUTEX_H_
#define BASE_MUTEX_H_

#include <memory>


struct MutexImpl;

class Mutex
{
    public:
        class SafeLocker
        {
            public:
                SafeLocker(Mutex * object)
                    :object_(object){ object_->lock(); }
                ~SafeLocker() { object_->unlock();}
            private:
                Mutex * object_;
        };

    public:
        Mutex();
        ~Mutex();
	
    private:
    	void * impl();


    public:
        void lock();
        void unlock();

    private:
        std::auto_ptr<MutexImpl> mutex_;
};


#endif
