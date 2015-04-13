#include <boost/thread/thread.hpp>
#include <stdio.h>

void thread_proc()
{
    printf("thread group test\n");
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


