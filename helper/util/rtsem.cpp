#include "rtsem.h"

#include <errno.h>
#include "log.h"

RTSem::RTSem():sem_(NULL) {
    sem_ = new sem_t;
    sem_init(sem_, 0, 0);    
}

RTSem::~RTSem() {
    if (sem_!=NULL) {
        sem_destroy(sem_);
        delete sem_;
    }
    sem_ = NULL;
}

bool RTSem::TimeWait(int wait_time_ms) { 
    if (sem_==NULL) {
        log_error("sem_==NULL");
        return false;
    }

    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);  

    struct timespec end_time;                                           
    end_time.tv_sec  = (cur_time.tv_sec+wait_time_ms/1000);   
    end_time.tv_nsec = (cur_time.tv_usec + wait_time_ms%1000*1000)*1000;     
    if (sem_timedwait(this->sem_, &end_time)!=0) {                
        log_error("TimeWait, errno=%d", errno);
        return false;
    }
    return true;
}

bool RTSem::Notify() {
    if (sem_==NULL) {
        log_error("sem_==NULL");
        return false;
    }
    if (sem_post(sem_)!=0) {
        log_error("Notify, errno=%d", errno);
        return false;
    }
    return true;
}



