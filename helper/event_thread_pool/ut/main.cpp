#include <stdio.h>
#include <stdlib.h>

#include "EventThreadPool.h"

#include "log.h"

void task_fun(int task_id) 
{
    log_notice("tid=%ld\ttask_id=%d", pthread_self(), task_id);
}

int main(int argc, char* argv[])
{
    int thread_num = 10;

    util::EventThreadPoolPtr event_thread_pool_ptr(new util::EventThreadPool(thread_num));
    if (!event_thread_pool_ptr->Start()) {
        log_error("event_thread_pool_ptr->Start failed");
        exit(0);
    }

    for (int i=0; i<1000; i++) {
        util::TaskPtr task_ptr(new util::Task(std::tr1::bind(&task_fun, i))); 
        if (!event_thread_pool_ptr->AddTask(task_ptr)) {
            log_error("event_thread_pool_ptr->AddTask"); 
            exit(0);
        }
    }
    return 0;
}

