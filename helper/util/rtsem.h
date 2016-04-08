#ifndef _RT_SEM_H_
#define _RT_SEM_H_

#include <fcntl.h>   
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h> 
#include <sys/time.h>  
#include <tr1/memory>

class RTSem {
public:
    RTSem();
    ~RTSem();

public:
    bool TimeWait(int wait_time_ms);
    bool Notify(); 

private:
    sem_t* sem_;
};
typedef std::tr1::shared_ptr<RTSem> RTSemPtr;

#endif //_RT_SEM_H_
