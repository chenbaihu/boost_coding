/*
 * cpu 限制:
 * mkdir /sys/fs/cgroup/cpu/chenbaihu
 * ls /sys/fs/cgroup/cpu/chenbaihu/
 * 有如下目录，对应CPU资源的不同限制：
 * cgroup.clone_children cgroup.procs cpu.cfs_quota_us cpu.stat tasks 
 * cgroup.event_control  cpu.cfs_period_us  cpu.shares notify_on_release
 * 
 * cpu一个核上的百分比限制：
 * cat /sys/fs/cgroup/cpu/chenbaihu/cpu.cfs_quota_us    默认: -1 不限制，可以控制到十万分之几的粒度
 * 比如：
 * echo 20000 > /sys/fs/cgroup/cpu/chenbaihu/cpu.cfs_quota_us   限制到20%
 *
 * cpu核数限制：
 * 比如限制CPU只能使用#2核和#3核 
 * echo "2,3" > /sys/fs/cgroup/cpuset/haoel/cpuset.cpus
 *
 * 限制方法：
 * 将pid添加到/sys/fs/cgroup/cpu/chenbaihu/tasks
 * 例如：echo 3529 >> /sys/fs/cgroup/cpu/chenbaihu/tasks
 */

#define _GNU_SOURCE         /* See feature_test_macros(7) */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

const int NUM_THREADS = 5;

void *thread_main(void *threadid)
{
    /* 把自己加入cgroup中（syscall(SYS_gettid)为得到线程的系统tid） */
    char cmd[128];
    sprintf(cmd, "echo %ld >> /sys/fs/cgroup/cpu/chenbaihu/tasks", syscall(SYS_gettid));
    system(cmd); 
    sprintf(cmd, "echo %ld >> /sys/fs/cgroup/cpuset/chenbaihu/tasks", syscall(SYS_gettid));
    system(cmd);

    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld, pid #%ld!\n", tid, syscall(SYS_gettid));

    int a=0; 
    while(1) {
        a++;
    }
    pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
    int num_threads;
    if (argc > 1){
        num_threads = atoi(argv[1]);
    }
    if (num_threads<=0 || num_threads>=100){
        num_threads = NUM_THREADS;
    }

    /* 设置CPU利用率为50% */
    mkdir("/sys/fs/cgroup/cpu/haoel", 755);
    system("echo 50000 > /sys/fs/cgroup/cpu/chenbaihu/cpu.cfs_quota_us");

    mkdir("/sys/fs/cgroup/cpuset/haoel", 755);
    /* 限制CPU只能使用#2核和#3核 */
    system("echo \"2,3\" > /sys/fs/cgroup/cpuset/chenbaihu/cpuset.cpus");

    pthread_t* threads = (pthread_t*) malloc (sizeof(pthread_t)*num_threads);
    int rc;
    long t;
    for(t=0; t<num_threads; t++){
        printf("In main: creating thread %ld\n", t);
        rc = pthread_create(&threads[t], NULL, thread_main, (void *)t);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    /* Last thing that main() should do */
    pthread_exit(NULL);
    free(threads);
}

