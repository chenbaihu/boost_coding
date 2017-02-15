#ifndef _SINGLETON_H
#define _SINGLETON_H

#include <stdio.h>
#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <stdlib.h>

template<typename T>
class Singleton : boost::noncopyable
{
public:
    static T& instance()
    {
        pthread_once(&once_, &Singleton::init);
        return *instance_;
    }
private:
    Singleton();
    ~Singleton();

    static void init()
    {
        instance_ = new T();
        ::atexit(destroy);
    }
    static void destroy()
    {
        typedef char has_define[(sizeof(T) == 0 ? -1:1)];
        has_define dummy;
        (void) dummy;

        delete instance_;
    }
private:
    static T* instance_;
    static pthread_once_t once_;
};

template<typename T>
T* Singleton<T>::instance_ = NULL;

template<typename T>
pthread_once_t Singleton<T>::once_ = PTHREAD_ONCE_INIT;

#endif
