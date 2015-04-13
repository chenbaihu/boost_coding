#include "semop.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

namespace util {

bool SemOP::Init() {
    union semun un;
    bzero(&un,sizeof(un));

    // get exist semaphore
    if ((id_=semget(key_, 0, 0)) > 0) {
        usleep(100);
        return true;
    }

    //semaphore does not exist - create
    if ((id_=semget(key_, 1, IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) != -1) {
        //initialize the semaphore 
        un.val = 1;
        if (semctl(id_, 0, SETVAL, un) != 0)
        {
            snprintf(errbuf_, sizeof(errbuf_), "Init semctl key[%d] error[%d][%s]", key_, errno, strerror(errno));
            return false;
        } 
        return true;
    }

    if (errno==EEXIST) {
        if ((id_=semget(key_, 0, 0)) != -1) {
            usleep(100);
            return true;
        }
        snprintf(errbuf_, sizeof(errbuf_), "Init semget key[%d] error[%d][%s],after check create", key_, errno, strerror(errno));
        return false;
    }
    snprintf(errbuf_, sizeof(errbuf_), "Init semget key[%d] error[%d][%s],first create", key_, errno, strerror(errno));
    return false;
}

int SemOP::op(int num) {
    struct sembuf sb[1];
    sb[0].sem_num = 0;
    sb[0].sem_op  = num;
    sb[0].sem_flg = SEM_UNDO ;

    // int semop(int semid, struct sembuf *sops, unsigned nsops);
    //
    // int semtimedop(int  semid, struct sembuf *sops, unsigned nsops, struct timespec *timeout);
    //
    // semid 信号集的识别码，可通过semget获取。
    // 
    // sops：指向存储信号操作结构的数组指针，信号操作结构的原型如下:
    // struct sembuf
    // {
    //     unsigned short sem_num;  /* semaphore number //      操作信号在信号集中的编号，第一个信号的编号是0。*/
    //     short          sem_op;   /* semaphore operation 
    //                                  如果其值为正数，该值会加到现有的信号内含值中。通常用于释放所控资源的使用权；
    //                                  如果sem_op的值为负数，而其绝对值又大于信号的现值，操作将会阻塞，直到信号值大于或等于sem_op的绝对值。
    //                                  通常用于获取资源的使用权；如果sem_op的值为0，则操作将暂时阻塞，直到信号的值变为0。*/
    //     short          sem_flg;  /* operation flags 
    //                                 信号操作标志，可能的选择有两种:
    //                                 IPC_NOWAIT //对信号的操作不能满足时，semop()不会阻塞，并立即返回，同时设定错误信息。
    //                                 IPC_UNDO   //程序结束时(不论正常或不正常)，保证信号值会被重设为semop()调用前的值。
    //                                            //这样做的目的在于避免程序在异常情况下结束时未将锁定的资源解锁，造成该资源永远锁定。
    //                              */
    // };
    // nsops：信号操作结构的数量，恒大于或等于1。
    //
    // timeout：当semtimedop()调用致使进程进入睡眠时，睡眠时间不能超过本参数指定的值。如果睡眠超时，semtimedop()将失败返回，并设定错误值为EAGAIN。
    //          如果本参数的值为NULL，semtimedop()将永远睡眠等待。
    //
    return semop(id_, sb, 1);
}

bool SemOP::Lock() {
    if (op(-1) != 0) {
         snprintf(errbuf_, sizeof(errbuf_), "Lock key[%d] error[%d][%s]", key_, errno, strerror(errno));
         return false;
    }
    return true;
}

bool SemOP::UnLock() {
    if (op(1) != 0) {
         snprintf(errbuf_, sizeof(errbuf_), "UnLock key[%d] error[%d][%s]", key_, errno, strerror(errno));
         return false;
    }
    return true;
}

int SemOP::Ctl(int semnum, int cmd, semun un) {
    return semctl(id_, semnum, cmd, un);
}

} // end of namespace util


#ifdef _DEBUG_

int main(int argc, char* argv[])
{
    key_t key = ftok("/dev/null", 0); 
    util::SemOP sem_op(key);
    if (!sem_op.Init()) {
        fprintf(stderr, "sem_op Init failed, err[%s\n]", sem_op.GetErr());
        return 1;
    }

    if (sem_op.Lock()) {
        fprintf(stdout, "sem_op Lock succ\n");
    }

    if (!sem_op.Lock()) {
        fprintf(stderr, "sem_op Lock failed\n");
    }

    if (!sem_op.UnLock()) {
        fprintf(stderr, "sem_op UnLock failed\n");
    }

    return 0;
}

#endif

