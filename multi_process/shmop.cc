#include "shmop.h"

namespace util {

bool ShmOP::Init(int key, size_t size, int shmflag) {
    if (init_) {
        fprintf(stderr, "ShmOP[%#x]::init fail:can't reinit shm", key);
        return false;
    }

    size += sizeof(ShmContext);
    int id = CreateShm(key, size, shmflag);  
    if (id < 0) {
        return false;
    }

    ptr_ = shmat(id, NULL, 0);
    if (((void*)-1) == ptr_) {
        fprintf(stderr, "ShmOP[%#x]::init shmat fail:[%d:%s]", key, errno, strerror(errno));
        ptr_ = NULL;
        return false;
    }

    ShmContext * tmp_ptr = (ShmContext*)ptr_; 
    if (create_) {
        bzero(ptr_,size);

        tmp_ptr->magic = ShmContext::ShmMagic;
        tmp_ptr->size = size;
        strncpy(tmp_ptr->version, ShmVersion, sizeof(tmp_ptr->version));
        init_ = true;
        return true;
    }
    
    int  count    = 0;
    bool check_ok = false;
    while (count < 3) {
        ++count;
        if (tmp_ptr->magic != ShmContext::ShmMagic) {
            fprintf(stderr, "ShmOP[%#x]::init shmat fail:[magic is not:%d]", key, ShmContext::ShmMagic); 
            usleep(1000*100);
            continue;
        }

        if (strncmp(tmp_ptr->version , ShmVersion, sizeof(tmp_ptr->version)) != 0) {
            fprintf(stderr, "ShmOP[%#x]::init shmat fail:[version is not:%s]",key, ShmVersion); 
            usleep(1000*100);
            continue;
        }

        if ((size != sizeof(ShmContext)) && (tmp_ptr->size != size)) {
            fprintf(stderr, "ShmOP[%#x]::init shmat fail:[size[%u] != reqsize[%ld]]", key, tmp_ptr->size, size); 
            usleep(1000*100);
            continue;
        }

        check_ok = true;
        break;
    }

    if (!check_ok) {
        shmdt(ptr_);
        ptr_ = NULL;
        return false;
    }

    init_ = true;
    return true;
}

int ShmOP::CreateShm(int key, size_t size, int shmflag) {
    int id = shmget(key, 0, 0);    //获取已经存在的共享内存
    if (id > 0) {
        return id;
    }
    
    if ((shmflag&IPC_CREAT)==0) {
        fprintf(stderr, "ShmOP[%#x]::CreateShm create fail:[%d:%s],no shm,but not create tag", key, errno, strerror(errno));
        return -1;
    } 

    id = shmget(key, size, shmflag|IPC_CREAT|IPC_EXCL);
    if (id > 0) {
        create_ = true;
        return id;
    }

    if (errno != EEXIST) {
        fprintf(stderr, "ShmOP[%#x]::CreateShm create fail:[%d:%s]", key, errno, strerror(errno));
        return -1;
    }

    id = shmget(key, 0, 0);
    if (id <= 0) {
        fprintf(stderr, "ShmOP[%#x]::CreateShm get fail:[%d:%s]", key, errno, strerror(errno));
        return -1;
    }
    return id;
}

} // end of namespace util


#ifdef _DEBUG_

int main(int argc, char* argv[])
{
    key_t key = ftok("/dev/null", 0);
    util::ShmOP shm_op;
    if (!shm_op.Init(key, 4*1024*1024)) {
        return 1;
    }

    char* p = (char*)shm_op.GetPtr();
    if (p!=NULL) {
        snprintf(p, 1024, "shm succ");
        return 0;
    }
    return 0;
}

#endif




