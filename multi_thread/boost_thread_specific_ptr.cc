#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>   // thread_specific_ptr
#include <cassert>
#include <stdio.h>

boost::thread_specific_ptr<int> value;

void increment()
{
    int* p = value.get();
    ++*p;
}

void thread_proc()
{
    value.reset(new int(0)); // initialize the thread's storage
    for (int i=0; i<10; ++i)
    {
        increment();
        int* p = value.get();
        assert(*p == i+1);
    }
    printf("thread_id=%ld\tvalue=%d\n", pthread_self(), *value); 
    fflush(stdout);
}

int main(int argc, char* argv[])
{
    boost::thread_group threads;

    for (int i=0; i<10; ++i) {
        threads.create_thread(&thread_proc);
    }

    threads.join_all();
}
