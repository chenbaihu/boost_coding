/*
 * mem 限制:
 * 创建一个mem cgroup
 * mkdir /sys/fs/cgroup/memory/chenbaihu/ 
 * echo 64k > /sys/fs/cgroup/memory/chenbaihu/memory.limit_in_bytes
 *
 * 将进程加入这个cgroup: /sys/fs/cgroup/memory/chenbaihu/tasks
 * 例如：echo 3529 >> /sys/fs/cgroup/memory/chenbaihu/tasks
 *
 * 该进程会因为内存问题被kill掉了
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    int size = 0;
    int chunk_size = 512;
    void *p = NULL;

    while(1) {

        if ((p = malloc(p, chunk_size)) == NULL) {
            printf("out of memory!!\n");
            break;
        }
        memset(p, 1, chunk_size);
        size += chunk_size;
        printf("[%d] - memory is allocated [%8d] bytes \n", getpid(), size);
        sleep(1);
    }
    return 0;
}

