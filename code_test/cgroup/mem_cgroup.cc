/*
 * mem ����:
 * ����һ��mem cgroup
 * mkdir /sys/fs/cgroup/memory/chenbaihu/ 
 * echo 64k > /sys/fs/cgroup/memory/chenbaihu/memory.limit_in_bytes
 *
 * �����̼������cgroup: /sys/fs/cgroup/memory/chenbaihu/tasks
 * ���磺echo 3529 >> /sys/fs/cgroup/memory/chenbaihu/tasks
 *
 * �ý��̻���Ϊ�ڴ����ⱻkill����
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

