#ifndef __SHMOP_H
#define __SHMOP_H

#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>

namespace util {

struct ShmContext {
	int magic;
	char version[16];
	unsigned int size;
	char reverse[8];

	const static int ShmMagic = 0x47aedc32;
	#define ShmVersion "shmv1.0"
};

class ShmOP {
public:
    ShmOP() {
        ptr_  = NULL; 
        init_ = false; 
        create_=false; 
    }
    
    ~ShmOP() {
    	if (ptr_) {
    		shmdt(ptr_);
            ptr_ = NULL;
    	}
    }
    
    bool Init(int key, size_t size, int shmflag=0664|IPC_CREAT);
    
    void * GetPtr() {
    	if (ptr_ != NULL) {
    		return ((char*)ptr_)+sizeof(ShmContext);
    	} else {
    		return NULL;
    	}
    }

private: 
   int CreateShm(int key, size_t size, int shmflag); 

private:
	void* ptr_;
	bool init_;
	bool create_;
};

} // end of nsmespace util
#endif


