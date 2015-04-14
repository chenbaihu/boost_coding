#ifndef __THREAD_H
#define __THREAD_H

#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <stdint.h>
#include <tr1/functional>

namespace util {

class Thread {
    public:
        pthread_t tid_;
        typedef std::tr1::function<void()> Function;
        Thread(const Function& function);
        ~Thread();

        bool Start();
        void Join();

        pthread_t tid() const { return tid_; };

        bool IsRuning() const { return tid_!=0; }

    private:
        Function function_;
        static void* ThreadMain(void*);
        
        // 不允许copy
        Thread(const Thread&);
        void operator=(const Thread&);
};

} // namespace util
                                                                            
#endif
