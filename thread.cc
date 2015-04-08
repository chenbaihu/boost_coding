#include <boost/thread/thread.hpp>
#include <stdio.h>

void thread_proc()
{
    printf("thread test\n");
    fflush(stdout);
}

int main(int argc, char* argv[])
{
    boost::thread thread(&thread_proc);

    thread.join();
}


