#include "thread.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>

namespace util {

Thread::Thread(const Function& function)
    :tid_(0), function_(function)  
{
}

Thread::~Thread() 
{
    if (IsRuning()) {
        Join();
    }
}

bool Thread::Start() {
    int result = pthread_create(&tid_, NULL, Thread::ThreadMain,
            (void*)this);
    if (result != 0) {
        tid_ = 0;
        fprintf(stderr, "pthread_create: %s\n", strerror(result));
        return false;
    }
    return true;
}

void Thread::Join() {
    if (!IsRuning()) {
        return;
    }

    int result = pthread_join(tid_, NULL);
    if (result != 0) {
        fprintf(stderr, "pthread_join: %s\n", strerror(result));
        return; // abort
    }
    tid_ = 0;
}

void* Thread::ThreadMain(void* arg) {
    Thread* thread = static_cast<Thread*>(arg);
    try {
        thread->function_();
    } catch (const std::exception& e) {
        //fprintf(stderr, "ThreadMain throws: %s\n", e.what().str().c_str());
    } catch (...) {
        //fprintf(stderr, "ThreadMain throws: %s\n", e.what().str().c_str());
    }
    return NULL;
}

} // namespace util

#ifdef _DEBUG_

void thread_proc()
{
    while(true) {
        printf("thread test\n");
        fflush(stdout);
        sleep(1);
    }
}

int main(int argc, char* argv[])
{
    util::Thread thread(std::tr1::bind(&thread_proc));
    thread.Start();
    thread.Join();
    return 0;
}

#endif 

